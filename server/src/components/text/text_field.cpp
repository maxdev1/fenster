// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/text/text_field.hpp"
#include "components/cursor.hpp"
#include "components/text/move/default_caret_move_strategy.hpp"
#include "events/key_event.hpp"
#include "events/mouse_event.hpp"

#include <libproperties/properties.hpp>
#include <libfenster/properties.hpp>
#include <libfenster/font/font_loader.hpp>
#include <sstream>

namespace fensterserver
{
	TextField::TextField() :
		text(""), cursor(0), marker(0), scrollX(0), secure(false),
		visualStatus(TextFieldVisualStatus::NORMAL), fontSize(14),
		textColor(_RGB(0, 0, 0)), insets(fenster::Insets(5, 5, 5, 5))
	{
		caretMoveStrategy = DefaultCaretMoveStrategy::getInstance();
		viewModel = fenster::TextLayouter::getInstance()->initializeBuffer();
		font = fenster::FontLoader::getDefault();
	}

	void TextField::setText(std::string newText)
	{
		text = newText;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	std::string TextField::getText()
	{
		return text;
	}

	void TextField::setSecure(bool newSecure)
	{
		secure = newSecure;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	void TextField::update()
	{
		fenster::Rectangle bounds = getBounds();

		std::string visible_text = text;
		if(secure)
		{
			visible_text = "";
			for(int i = 0; i < text.length(); i++)
			{
				visible_text += "*";
			}
		}

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		fenster::TextLayouter::getInstance()->layout(cr, visible_text.c_str(), font, fontSize,
		                                             fenster::Rectangle(0, 0, bounds.width, bounds.height),
		                                             fenster::TextAlignment::LEFT,
		                                             viewModel, false);

		markFor(COMPONENT_REQUIREMENT_PAINT);

		graphics.releaseContext();
	}

	void TextField::paint()
	{
		if(font == nullptr)
			return;

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		fenster::Rectangle bounds = getBounds();

		// background
		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_rectangle(cr, 0, 0, bounds.width, bounds.height);
		cairo_fill(cr);

		// border
		fenster::ColorArgb borderColor;
		if(focused)
		{
			borderColor = _RGB(55, 155, 255);
		}
		else if(visualStatus == TextFieldVisualStatus::HOVERED)
		{
			borderColor = _RGB(150, 150, 150);
		}
		else
		{
			borderColor = _RGB(180, 180, 180);
		}
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(borderColor));
		cairo_rectangle(cr, 0.5, 0.5, bounds.width - 1, bounds.height - 1);
		cairo_set_line_width(cr, 1.0);
		cairo_stroke(cr);

		// TODO Unfortunately need to do this again because we have a new cairo context otherwise they are lost
		std::string visible_text = text;
		if(secure)
		{
			visible_text = "";
			for(int i = 0; i < text.length(); i++)
			{
				visible_text += "*";
			}
		}
		fenster::TextLayouter::getInstance()->layout(cr, visible_text.c_str(), font, fontSize,
		                                             fenster::Rectangle(0, 0, bounds.width, bounds.height),
		                                             fenster::TextAlignment::LEFT,
		                                             viewModel, false);

		// Scroll
		applyScroll();

		int pos = 0;
		int first = marker < cursor ? marker : cursor;
		int second = marker > cursor ? marker : cursor;

		// Paint marking
		if(focused)
		{
			pos = 0;
			for(pos = 0; pos < viewModel->positions.size(); pos++)
			{
				fenster::ColorArgb color = textColor;
				if(first != second && pos >= first && pos < second)
				{
					cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(55, 155, 255)));
					fenster::Rectangle before = positionToCursorBounds(pos);
					fenster::Rectangle after = positionToCursorBounds(pos + 1);
					cairo_rectangle(cr, before.x, before.y, after.x - before.x, before.height);
					cairo_fill(cr);

					color = _RGB(255, 255, 255);
				}
			}
		}

		// Paint glyphs
		pos = 0;
		for(fenster::PositionedGlyph& g: viewModel->positions)
		{

			fenster::Rectangle onView = glyphToView(g);
			fenster::ColorArgb color = textColor;
			if(focused && first != second && pos >= first && pos < second)
			{
				color = _RGB(255, 255, 255);
			}

			cairo_save(cr);
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(color));
			cairo_translate(cr, onView.x - g.glyph->x, onView.y - g.glyph->y); // TODO?
			cairo_glyph_path(cr, g.glyph, g.glyph_count);
			cairo_fill(cr);
			cairo_restore(cr);
			++pos;
		}

		// Paint cursor
		if(focused)
		{
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(60, 60, 60)));
			auto cursorBounds = positionToCursorBounds(cursor);
			cairo_rectangle(cr, cursorBounds.x, cursorBounds.y, cursorBounds.width, cursorBounds.height);
			cairo_fill(cr);
		}

		graphics.releaseContext();
	}

	void TextField::applyScroll()
	{
		int cursorX = positionToUnscrolledCursorX(cursor);
		fenster::Rectangle bounds = getBounds();

		int textUsableWidth = bounds.width - insets.left - insets.right;

		if(scrollX + cursorX > textUsableWidth)
			scrollX = textUsableWidth - cursorX;
		else if(scrollX + cursorX < insets.left)
			scrollX = -cursorX + insets.left;

		if(viewModel->textBounds.width > textUsableWidth)
			scrollX = std::max(textUsableWidth - viewModel->textBounds.width,
			                   std::min(scrollX, 0));
		else
			scrollX = 0;
	}

	int TextField::positionToUnscrolledCursorX(int pos)
	{

		int cursorX = insets.left;
		int positionsCount = viewModel->positions.size();
		for(int i = 0; i < positionsCount; i++)
		{
			fenster::PositionedGlyph& g = viewModel->positions[i];
			// After last?
			if(i == positionsCount - 1 && pos == positionsCount)
			{
				cursorX = g.position.x + insets.left + g.advance.x;
			}
			// Anywhere inside
			if(i == pos)
			{
				cursorX = g.position.x + insets.left - 1;
			}
		}

		return cursorX;
	}

	void TextField::setCursor(int pos)
	{
		cursor = pos;
		if(cursor < 0)
		{
			cursor = 0;
		}
		if(cursor > text.length())
		{
			cursor = text.length();
		}

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void TextField::setMarker(int pos)
	{
		marker = pos;
		if(marker < 0)
		{
			marker = 0;
		}
		if(marker > text.length())
		{
			marker = text.length();
		}

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void TextField::backspace(fenster::KeyInfo& info)
	{

		if(text.length() > 0)
		{

			fenster::Range selected = getSelectedRange();

			int leftcut = selected.getFirst();
			if(info.alt)
			{
				leftcut = caretMoveStrategy->calculateSkip(text, leftcut, CaretDirection::LEFT);
			}
			else if(info.ctrl)
			{
				leftcut = 0;
			}

			int rightcut = selected.getLast();

			if(rightcut - leftcut == 0)
			{
				leftcut--;
			}

			if(leftcut >= 0 && rightcut <= text.length())
			{
				std::string beforeCursor = text.substr(0, leftcut);
				std::string afterCursor = text.substr(rightcut);
				text = beforeCursor + afterCursor;
				setCursor(leftcut);
				setMarker(leftcut);

				markFor(COMPONENT_REQUIREMENT_UPDATE);
			}
		}
	}

	void TextField::insert(std::string ins)
	{

		fenster::Range selected = getSelectedRange();

		int first = selected.getFirst();
		int last = selected.getLast();

		if(first < 0)
		{
			first = 0;
		}
		if(last > text.size())
		{
			last = text.size();
		}

		std::string beforeCursor = text.substr(0, first);
		std::string afterCursor = text.substr(last);

		text = beforeCursor + ins + afterCursor;
		setCursor(first + ins.length());
		setMarker(first + ins.length());
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	Component* TextField::handleKeyEvent(KeyEvent& ke)
	{
		if(ke.info.key == "KEY_SHIFT_L")
		{
			shiftDown = ke.info.pressed;
		}

		if(ke.info.pressed)
		{
			if(ke.info.key == "KEY_BACKSPACE")
			{
				backspace(ke.info);
			}
			else if(ke.info.key == "KEY_ARROW_LEFT")
			{
				caretMoveStrategy->moveCaret(this, CaretDirection::LEFT, ke.info);
			}
			else if(ke.info.key == "KEY_ARROW_RIGHT")
			{
				caretMoveStrategy->moveCaret(this, CaretDirection::RIGHT, ke.info);
			}
			else if(ke.info.key == "KEY_A" && ke.info.ctrl)
			{
				marker = 0;
				cursor = text.length();
				markFor(COMPONENT_REQUIREMENT_PAINT);
			}
			else
			{
				char c = platformCharForKey(ke.info);

				if(c != -1)
				{
					std::stringstream s;
					s << c;
					insert(s.str());
				}
			}
		}
		sendKeyEventToListener(ke);
		return this;
	}

	void TextField::setFocusedInternal(bool focused)
	{
		FocusableComponent::setFocusedInternal(focused);
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	Component* TextField::handleMouseEvent(MouseEvent& me)
	{
		if(me.type == FENSTER_MOUSE_EVENT_ENTER)
		{
			visualStatus = TextFieldVisualStatus::HOVERED;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			Cursor::set("text");
			return this;
		}
		else if(me.type == FENSTER_MOUSE_EVENT_LEAVE)
		{
			visualStatus = TextFieldVisualStatus::NORMAL;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			Cursor::set("default");
			return this;
		}
		else if(me.type == FENSTER_MOUSE_EVENT_PRESS)
		{

			fenster::Point p = me.position;
			int clickCursor = viewToPosition(p);

			if(me.clickCount > 2)
			{
				marker = 0;
				cursor = text.length();
			}
			else if(me.clickCount == 2)
			{
				marker = caretMoveStrategy->calculateSkip(text, clickCursor, CaretDirection::LEFT);
				cursor = caretMoveStrategy->calculateSkip(text, clickCursor, CaretDirection::RIGHT);
			}
			else
			{
				cursor = clickCursor;
				if(!shiftDown)
				{
					marker = cursor;
				}
			}

			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == FENSTER_MOUSE_EVENT_DRAG)
		{
			fenster::Point p = me.position;
			cursor = viewToPosition(p);
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}

		if(me.type != FENSTER_MOUSE_EVENT_SCROLL)
		{
			// Swallow other events so we can handle the cursor
			return this;
		}
		return nullptr;
	}

	int TextField::viewToPosition(fenster::Point p)
	{
		int pos = 0;

		for(int i = 0; i < viewModel->positions.size(); i++)
		{
			fenster::PositionedGlyph g = viewModel->positions[i];
			fenster::Rectangle onView = glyphToView(g);

			if(p.x < onView.x + onView.width / 2)
			{
				break;
			}

			++pos;
		}
		return pos;
	}

	fenster::Rectangle TextField::glyphToView(fenster::PositionedGlyph& g)
	{
		int yOffset = getBounds().height / 2 - fontSize / 2 - 2;
		int x = scrollX + g.position.x + insets.left;
		int y = g.position.y + yOffset;
		return fenster::Rectangle(x, y, g.size.width, g.size.height);
	}

	fenster::Rectangle TextField::positionToCursorBounds(int pos)
	{

		int cursorX = positionToUnscrolledCursorX(pos);
		int caretHeight = fontSize + 4;
		return fenster::Rectangle(scrollX + cursorX, getBounds().height / 2 - caretHeight / 2, 1, caretHeight);
	}

	void TextField::setFont(fenster::Font* f)
	{
		font = f;
	}

	fenster::Range TextField::getSelectedRange()
	{
		return fenster::Range(marker, cursor);
	}

	bool TextField::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Secure)
		{
			*out = secure;
			return true;
		}

		return false;
	}

	bool TextField::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Secure)
		{
			secure = value;
			return true;
		}

		return Component::setNumericProperty(property, value);
	}
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/text/text_box.hpp"
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
	TextBox::TextBox()
	{
		caretMoveStrategy = DefaultCaretMoveStrategy::getInstance();
		viewModel = fenster::TextLayouter::initializeLayout();
		font = fenster::FontLoader::getDefault();
	}

	void TextBox::setText(std::string newText)
	{
		text = newText;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	std::string TextBox::getText()
	{
		return text;
	}

	void TextBox::setSecure(bool secure)
	{
		this->secure = secure;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	void TextBox::setMultiLine(bool multiline)
	{
		this->multiline = multiline;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}


	void TextBox::update()
	{
		auto cr = graphics.acquireContext();
		if (!cr)
			return;

		layoutText(cr);
		this->setPreferredSize(fenster::Dimension(
			viewModel->textBounds.width + insets.left + insets.right,
			viewModel->textBounds.height + insets.top + insets.bottom
		));

		markFor(COMPONENT_REQUIREMENT_PAINT);
		graphics.releaseContext();
	}

	void TextBox::paintBackground(cairo_t* cr)
	{
		auto bounds = getBounds();

		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_rectangle(cr, 0, 0, bounds.width, bounds.height);
		cairo_fill(cr);

		fenster::ColorArgb borderColor;
		if (focused)
			borderColor = _RGB(55, 155, 255);
		else if (visualStatus == TextBoxVisualStatus::HOVERED)
			borderColor = _RGB(150, 150, 150);
		else
			borderColor = _RGB(180, 180, 180);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(borderColor));
		cairo_rectangle(cr, 0.5, 0.5, bounds.width - 1, bounds.height - 1);
		cairo_set_line_width(cr, 1.0);
		cairo_stroke(cr);
	}

	void TextBox::layoutText(cairo_t* cr)
	{
		fenster::Rectangle bounds = getBounds();
		std::string visibleText = text;
		if (secure)
		{
			visibleText = "";
			for (int i = 0; i < text.length(); i++)
				visibleText += "*";
		}
		fenster::Rectangle textBounds(0, 0, bounds.width, bounds.height);
		fenster::TextLayouter::layout(cr, visibleText.c_str(), font, fontSize,
		                              textBounds,
		                              fenster::TextAlignment::LEFT,
		                              viewModel, false);
	}

	void TextBox::paintMarking(cairo_t* cr)
	{
		int first = marker < cursor ? marker : cursor;
		int second = marker > cursor ? marker : cursor;

		int pos = 0;
		for (pos = 0; pos < viewModel->positions.size(); pos++)
		{
			if (first != second && pos >= first && pos < second)
			{
				cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(55, 155, 255)));
				fenster::Rectangle before = positionToCursorBounds(pos);
				fenster::Rectangle after = positionToCursorBounds(pos + 1);

				if (before.y == after.y)
				{
					cairo_rectangle(cr, before.x, before.y, after.x - before.x, before.height);
					cairo_fill(cr);
				} else
				{
					// LF
					cairo_rectangle(cr, before.x, before.y, 4, before.height);
					cairo_fill(cr);
				}
			}
		}
	}

	void TextBox::paintText(cairo_t* cr)
	{
		int selectStart = std::min(marker, cursor);
		int selectEnd = std::max(marker, cursor);

		for (int pos = 0; pos < viewModel->positions.size(); pos++)
		{
			auto g = viewModel->positions[pos];

			if (g.controlChar == -1)
			{
				fenster::ColorArgb color = textColor;
				if (focused && selectStart != selectEnd && pos >= selectStart && pos < selectEnd)
					color = _RGB(255, 255, 255);

				cairo_save(cr);
				cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(color));
				fenster::TextLayouter::paintChar(
					cr, g, fenster::Point(scrollX + insets.left, scrollY + insets.top));
				cairo_restore(cr);
			}
		}
	}

	void TextBox::paintCursor(cairo_t* cr)
	{
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(60, 60, 60)));
		auto cursorBounds = positionToCursorBounds(cursor);
		cairo_rectangle(cr, cursorBounds.x, cursorBounds.y, cursorBounds.width, cursorBounds.height);
		cairo_fill(cr);
	}

	void TextBox::paint()
	{
		if (font == nullptr)
			return;

		auto cr = graphics.acquireContext();
		if (!cr)
			return;

		if (scrollToCursorBeforePaint)
		{
			scrollToCursorBeforePaint = false;
			scrollToCursor();
		}

		paintBackground(cr);
		layoutText(cr);
		if (focused)
			paintMarking(cr);
		paintText(cr);
		if (focused)
			paintCursor(cr);

		graphics.releaseContext();
	}

	void TextBox::limitScroll()
	{
		int textUsableWidth = bounds.width - insets.left - insets.right;
		if (viewModel->textBounds.width > textUsableWidth)
			scrollX = std::max(textUsableWidth - viewModel->textBounds.width,
			                   std::min(scrollX, 0));
		else
			scrollX = 0;

		int textUsableHeight = bounds.height - insets.top - insets.bottom;
		if (viewModel->textBounds.height > textUsableHeight)
			scrollY = std::max(textUsableHeight - viewModel->textBounds.height - fontSize, std::min(scrollY, 0));
		else
			scrollY = 0;
	}

	void TextBox::scrollToCursor()
	{
		auto cursorPos = positionToUnscrolledCursorPoint(cursor);
		fenster::Rectangle bounds = getBounds();

		int textUsableWidth = bounds.width - insets.left - insets.right;
		if (scrollX + cursorPos.x > textUsableWidth)
			scrollX = textUsableWidth - cursorPos.x;
		else if (scrollX + cursorPos.x < insets.left)
			scrollX = -cursorPos.x + insets.left;

		int cursorTop = cursorPos.y - fontSize / 2;

		int textUsableHeight = bounds.height - insets.top - insets.bottom;
		if (scrollY + cursorTop < insets.top)
			scrollY = -cursorTop + insets.top;
		else if (scrollY + cursorPos.y > textUsableHeight)
			scrollY = textUsableHeight - cursorPos.y;

		limitScroll();
	}

	void TextBox::setCursor(int pos)
	{
		cursor = pos;
		if (cursor < 0)
		{
			cursor = 0;
		}
		if (cursor > text.length())
		{
			cursor = text.length();
		}

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void TextBox::setMarker(int pos)
	{
		marker = pos;
		if (marker < 0)
		{
			marker = 0;
		}
		if (marker > text.length())
		{
			marker = text.length();
		}

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void TextBox::backspace(fenster::KeyInfo& info)
	{
		if (text.empty())
			return;

		fenster::Range selected = getSelectedRange();

		int leftCut = selected.getFirst();
		if (info.alt)
		{
			leftCut = caretMoveStrategy->calculateSkip(text, leftCut, CaretDirection::LEFT);
		} else if (info.ctrl)
		{
			leftCut = 0;
		}

		int rightCut = selected.getLast();
		if (rightCut - leftCut == 0)
		{
			leftCut--;
		}

		if (leftCut >= 0 && rightCut <= text.length())
		{
			std::string beforeCursor = text.substr(0, leftCut);
			std::string afterCursor = text.substr(rightCut);
			text = beforeCursor + afterCursor;
			setCursor(leftCut);
			setMarker(leftCut);

			markFor(COMPONENT_REQUIREMENT_UPDATE);
		}
	}

	void TextBox::insert(std::string insertedText)
	{
		fenster::Range selected = getSelectedRange();

		int32_t first = selected.getFirst();
		int32_t last = selected.getLast();

		if (first < 0)
			first = 0;
		if (last > text.size())
			last = text.size();

		std::string beforeCursor = text.substr(0, first);
		std::string afterCursor = text.substr(last);

		text = beforeCursor + insertedText + afterCursor;
		setCursor(first + insertedText.length());
		setMarker(first + insertedText.length());
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	Component* TextBox::handleKeyEvent(KeyEvent& ke)
	{
		if (ke.info.key == "KEY_SHIFT_L")
		{
			shiftDown = ke.info.pressed;
		}

		if (ke.info.pressed)
		{
			if (ke.info.key == "KEY_BACKSPACE")
			{
				backspace(ke.info);
			} else if (ke.info.key == "KEY_ARROW_LEFT")
			{
				caretMoveStrategy->moveCaret(this, CaretDirection::LEFT, ke.info);
			} else if (ke.info.key == "KEY_ARROW_RIGHT")
			{
				caretMoveStrategy->moveCaret(this, CaretDirection::RIGHT, ke.info);
			} else if (ke.info.key == "KEY_ARROW_UP")
			{
				auto currentBounds = positionToCursorBounds(cursor);
				if (currentBounds.y - scrollY > fontSize)
				{
					currentBounds.x += 2;
					currentBounds.y -= fontSize / 2;
					cursor = viewToPosition(currentBounds.getStart());
				} else
				{
					cursor = 0;
				}
				if (!shiftDown)
				{
					marker = cursor;
				}
				markFor(COMPONENT_REQUIREMENT_PAINT);
			} else if (ke.info.key == "KEY_ARROW_DOWN")
			{
				auto currentBounds = positionToCursorBounds(cursor);
				currentBounds.x += 2;
				currentBounds.y += fontSize / 2;
				cursor = viewToPosition(currentBounds.getEnd());
				if (!shiftDown)
				{
					marker = cursor;
				}
				markFor(COMPONENT_REQUIREMENT_PAINT);
			} else if (ke.info.key == "KEY_A" && ke.info.ctrl)
			{
				marker = 0;
				cursor = text.length();
				markFor(COMPONENT_REQUIREMENT_PAINT);
			} else
			{
				char c = platformCharForKey(ke.info);

				if (!multiline && c == '\n')
				{
					//
				} else if (c != -1)
				{
					std::stringstream s;
					s << c;
					insert(s.str());
				}
			}
		}

		scrollToCursorBeforePaint = true;
		sendKeyEventToListener(ke);
		return this;
	}

	void TextBox::setFocusedInternal(bool focused)
	{
		FocusableComponent::setFocusedInternal(focused);
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	Component* TextBox::handleMouseEvent(MouseEvent& me)
	{
		if (me.type == fenster::MouseEventType::Enter)
		{
			visualStatus = TextBoxVisualStatus::HOVERED;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			Cursor::set("text");
			return this;
		}

		if (me.type == fenster::MouseEventType::Leave)
		{
			visualStatus = TextBoxVisualStatus::NORMAL;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			Cursor::set("default");
			return this;
		}

		if (me.type == fenster::MouseEventType::Press)
		{
			fenster::Point p = me.position;
			int clickCursor = viewToPosition(p);

			if (me.clickCount > 2)
			{
				marker = 0;
				cursor = text.length();
			} else if (me.clickCount == 2)
			{
				marker = caretMoveStrategy->calculateSkip(text, clickCursor, CaretDirection::LEFT);
				cursor = caretMoveStrategy->calculateSkip(text, clickCursor, CaretDirection::RIGHT);
			} else
			{
				cursor = clickCursor;
				if (!shiftDown)
				{
					marker = cursor;
				}
			}

			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}

		if (me.type == fenster::MouseEventType::Drag)
		{
			fenster::Point p = me.position;
			cursor = viewToPosition(p);
			scrollToCursor();
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}

		if (me.type == fenster::MouseEventType::Scroll)
		{
			scrollY -= me.scroll * fontSize;
			limitScroll();
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}

		if (me.type != fenster::MouseEventType::Scroll)
		{
			// Swallow other events so we can handle the cursor
			return this;
		}
		return nullptr;
	}

	int TextBox::viewToPosition(fenster::Point p)
	{
		if (p.x < insets.left)
			p.x = insets.left;
		if (p.y < insets.top)
			p.y = insets.top;

		for (int i = 0; i < viewModel->positions.size(); i++)
		{
			auto cursorAt = positionToCursorBounds(i);
			auto cursorAfter = positionToCursorBounds(i + 1);

			if (cursorAfter.y == cursorAt.y) // same line
			{
				cursorAt.extend(cursorAfter.getStart());
				cursorAt.extend(cursorAfter.getEnd());
				if (cursorAt.contains(p))
					return i;
			} else if (p.y > cursorAt.y && p.y < cursorAt.y + cursorAt.height && p.x > cursorAt.x)
			{
				// new line
				return i;
			}
		}
		return p.y > insets.top ? viewModel->positions.size() : 0;
	}

	fenster::Point TextBox::positionToUnscrolledCursorPoint(int pos)
	{
		int cursorX = insets.left;
		int cursorY = insets.top;

		int positionsCount = viewModel->positions.size();
		for (int i = 0; i < positionsCount; i++)
		{
			fenster::PositionedGlyph& g = viewModel->positions[i];
			// After last?
			if (i == positionsCount - 1 && pos == positionsCount)
			{
				cursorX = insets.left + g.bounds.x + g.bounds.width;
				cursorY = insets.top + g.bounds.y;
				fenster::platformLog("last width %i", g.bounds.width);

				if (g.controlChar == '\n')
				{
					cursorX = insets.left;
					cursorY = insets.top + g.bounds.y + g.bounds.height;
				}
			}
			// Anywhere inside
			if (i == pos)
			{
				cursorX = insets.left + g.bounds.x - 1;
				cursorY = insets.top + g.bounds.y;
			}
		}

		fenster::Point cursorPoint;
		cursorPoint.x = cursorX;
		cursorPoint.y = cursorY;
		return cursorPoint;
	}

	fenster::Rectangle TextBox::positionToCursorBounds(int pos)
	{
		auto cursorPoint = positionToUnscrolledCursorPoint(pos);
		int lineHeight = fontSize + 4;
		return {
			cursorPoint.x + scrollX,
			cursorPoint.y + scrollY,
			1,
			lineHeight
		};
	}

	void TextBox::setFont(fenster::Font* f)
	{
		font = f;
	}

	fenster::Range TextBox::getSelectedRange()
	{
		return fenster::Range(marker, cursor);
	}

	bool TextBox::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if (property == fenster::ComponentProperty::Secure)
		{
			*out = secure;
			return true;
		}

		if (property == fenster::ComponentProperty::MultiLine)
		{
			*out = multiline ? 1 : 0;
			return true;
		}

		return false;
	}

	bool TextBox::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if (property == fenster::ComponentProperty::Secure)
		{
			setSecure(value == 1);
			return true;
		}

		if (property == fenster::ComponentProperty::MultiLine)
		{
			setMultiLine(value == 1);
			return true;
		}

		return Component::setNumericProperty(property, value);
	}
}

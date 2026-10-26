// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/label.hpp"
#include <libfenster/font/font_loader.hpp>
#include <libfenster/properties.hpp>

#include <cairo/cairo.h>

namespace fensterserver
{
	Label::Label()
	{
		Label::setFont(fenster::FontLoader::getDefault());
		graphics.resize(1, 1);
		viewModel = fenster::TextLayouter::initializeLayout();
	}

	Label::~Label()
	{
		fenster::TextLayouter::destroyLayout(viewModel);
	}

	void Label::setFont(fenster::Font* newFont)
	{
		fenster::platformAcquireMutex(lock);
		if(font != newFont) { font = newFont; textDirty = true; markFor(COMPONENT_REQUIREMENT_UPDATE); }
		fenster::platformReleaseMutex(lock);
	}

	void Label::layoutText(cairo_t* cr)
	{
		if(!textDirty || !font) return;
		// Measure natural text once. Alignment and widget width do not change
		// shaping or preferred size; they only change where cached glyphs draw.
		fenster::TextLayouter::layout(cr, text.c_str(), font, fontSize, {},
									 fenster::TextAlignment::LEFT, viewModel, false);
		lineWidths.clear();
		auto scaled = cairo_get_scaled_font(cr);
		for(size_t i = 0; i < viewModel->positions.size(); ++i)
		{
			auto& position = viewModel->positions[i];
			if(lineWidths.size() <= size_t(position.line)) lineWidths.resize(position.line + 1);
			if(i + 1 < viewModel->positions.size() && viewModel->positions[i + 1].line == position.line) continue;
			cairo_text_extents_t extents{};
			if(position.controlChar == -1)
				cairo_scaled_font_glyph_extents(scaled, position.glyph, position.glyphCount, &extents);
			lineWidths[position.line] = position.bounds.x + extents.x_advance;
		}
		textDirty = false;
		glyphWidth = -1;
	}

	void Label::positionGlyphs(int width)
	{
		if(glyphWidth == width) return;
		glyphs.clear();
		for(auto& position : viewModel->positions)
		{
			if(position.controlChar != -1) continue;
			int offset = alignment == fenster::TextAlignment::CENTER ? (width - lineWidths[position.line]) / 2 :
						 alignment == fenster::TextAlignment::RIGHT ? width - lineWidths[position.line] : 0;
			for(int i = 0; i < position.glyphCount; ++i)
			{
				auto glyph = position.glyph[i];
				glyph.x += position.renderPosition.x + offset;
				glyph.y += position.renderPosition.y;
				glyphs.push_back(glyph);
			}
		}
		glyphWidth = width;
	}

	void Label::update()
	{
		auto cr = graphics.acquireContext();
		if(!cr) return;
		layoutText(cr);
		if(font) setPreferredSize({viewModel->textBounds.width, viewModel->textBounds.height});
		graphics.releaseContext();
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void Label::paint()
	{
		clearSurface();
		auto cr = graphics.acquireContext();
		if(!cr) return;
		if(font)
		{
			layoutText(cr);
			positionGlyphs(bounds.width);
			cairo_set_font_face(cr, font->getFace());
			cairo_set_font_size(cr, fontSize);
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(color));
			cairo_save(cr);
			cairo_translate(cr, 0, (bounds.height - viewModel->textBounds.height) / 2);
			cairo_show_glyphs(cr, glyphs.data(), glyphs.size());
			cairo_restore(cr);
		}
		graphics.releaseContext();
	}

	void Label::setTitleInternal(std::string newText)
	{
		fenster::platformAcquireMutex(lock);
		if(text != newText) { text = std::move(newText); textDirty = true; markFor(COMPONENT_REQUIREMENT_UPDATE); }
		fenster::platformReleaseMutex(lock);
	}

	std::string Label::getTitle()
	{
		fenster::platformAcquireMutex(lock);
		auto result = text;
		fenster::platformReleaseMutex(lock);
		return result;
	}

	void Label::setAlignment(fenster::TextAlignment newAlignment)
	{
		fenster::platformAcquireMutex(lock);
		if(alignment != newAlignment) { alignment = newAlignment; glyphWidth = -1; markFor(COMPONENT_REQUIREMENT_PAINT); }
		fenster::platformReleaseMutex(lock);
	}

	fenster::TextAlignment Label::getAlignment()
	{
		return alignment;
	}

	fenster::ColorArgb Label::getColor()
	{
		return color;
	}

	void Label::setColor(fenster::ColorArgb color)
	{
		fenster::platformAcquireMutex(lock);
		if(this->color != color) { this->color = color; markFor(COMPONENT_REQUIREMENT_PAINT); }
		fenster::platformReleaseMutex(lock);
	}

	bool Label::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::FontSize) {
			auto size = std::max(8u, std::min(value, 48u));
			fenster::platformAcquireMutex(lock);
			if(fontSize != int(size)) { fontSize = size; textDirty = true; markFor(COMPONENT_REQUIREMENT_UPDATE); }
			fenster::platformReleaseMutex(lock);
			return true;
		}
		if (property == fenster::ComponentProperty::Color)
		{
			this->setColor(value);
			return true;
		}

		if (property == fenster::ComponentProperty::Alignment)
		{
			this->setAlignment((fenster::TextAlignment) value);
			return true;
		}

		return Component::setNumericProperty(property, value);
	}
}

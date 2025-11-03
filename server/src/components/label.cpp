// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/label.hpp"
#include <libfenster/font/font_loader.hpp>
#include <libfenster/properties.hpp>

#include <cairo/cairo.h>
#include <sstream>

namespace fensterserver
{
	Label::Label()
	{
		setFont(fenster::FontLoader::getDefault());
		alignment = fenster::TextAlignment::LEFT;
		color = _RGB(0, 0, 0);
		graphics.resize(1, 1);
	}

	void Label::setFont(fenster::Font* newFont)
	{
		font = newFont;
		fontSize = 14;
	}

	void Label::update()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		if(font)
		{
			cairo_save(cr);
			cairo_set_font_face(cr, font->getFace());
			cairo_set_font_size(cr, fontSize);
			cairo_text_extents(cr, this->text.c_str(), &textExtents);
			cairo_font_extents(cr, &fontExtents);
			cairo_restore(cr);
		}

		graphics.releaseContext();

		fenster::Dimension newPreferred(textExtents.width + 3, fontExtents.height + 3);

		// Set new preferred size
		auto oldPreferred = getPreferredSize();
		if(oldPreferred != newPreferred)
		{
			setPreferredSize(newPreferred);
			markParentFor(COMPONENT_REQUIREMENT_UPDATE);
		}
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void Label::paint()
	{
		clearSurface();

		auto bounds = getBounds();

		int textX;
		int textY = bounds.height / 2 - fontExtents.height / 2 + fontExtents.ascent;
		if(alignment == fenster::TextAlignment::CENTER)
		{
			textX = bounds.width / 2 - textExtents.width / 2;
		}
		else if(alignment == fenster::TextAlignment::RIGHT)
		{
			textX = bounds.width - textExtents.width;
		}
		else
		{
			textX = 0;
		}

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		if(font)
		{
			cairo_save(cr);
			cairo_set_source_rgb(cr, ARGB_FR_FROM(color), ARGB_FB_FROM(color), ARGB_FG_FROM(color));
			cairo_move_to(cr, textX, textY);
			cairo_set_font_face(cr, font->getFace());
			cairo_set_font_size(cr, fontSize);
			cairo_show_text(cr, text.c_str());
			cairo_restore(cr);
		}

		graphics.releaseContext();
	}

	void Label::setTitleInternal(std::string newText)
	{
		text = newText;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	std::string Label::getTitle()
	{
		return text;
	}

	void Label::setAlignment(fenster::TextAlignment newAlignment)
	{
		alignment = newAlignment;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
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
		this->color = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Label::setNumericProperty(int property, uint32_t value)
	{
		if(property == FENSTER_UI_PROPERTY_COLOR)
		{
			this->setColor(value);
			return true;
		}

		if(property == FENSTER_UI_PROPERTY_ALIGNMENT)
		{
			this->setAlignment((fenster::TextAlignment) value);
			return true;
		}

		return Component::setNumericProperty(property, value);
	}
}

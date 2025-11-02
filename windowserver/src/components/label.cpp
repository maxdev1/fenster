// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/label.hpp"
#include <libwindow/font/font_loader.hpp>
#include <libwindow/properties.hpp>

#include <cairo/cairo.h>
#include <sstream>
#include <windowserver.hpp>

label_t::label_t()
{
	setFont(g_font_loader::getDefault());
	alignment = g_text_alignment::LEFT;
	color = RGB(0, 0, 0);
	graphics.resize(1, 1);
}

void label_t::setFont(g_font* newFont)
{
	font = newFont;
	fontSize = 14;
}

void label_t::update()
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

	g_dimension newPreferred(textExtents.width + 3, fontExtents.height + 3);

	// Set new preferred size
	auto oldPreferred = getPreferredSize();
	if(oldPreferred != newPreferred)
	{
		setPreferredSize(newPreferred);
		markParentFor(COMPONENT_REQUIREMENT_UPDATE);
	}
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

void label_t::paint()
{
	clearSurface();

	auto bounds = getBounds();

	int textX;
	int textY = bounds.height / 2 - fontExtents.height / 2 + fontExtents.ascent;
	if(alignment == g_text_alignment::CENTER)
	{
		textX = bounds.width / 2 - textExtents.width / 2;
	}
	else if(alignment == g_text_alignment::RIGHT)
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

void label_t::setTitleInternal(std::string newText)
{
	text = newText;
	markFor(COMPONENT_REQUIREMENT_UPDATE);
}

std::string label_t::getTitle()
{
	return text;
}

void label_t::setAlignment(g_text_alignment newAlignment)
{
	alignment = newAlignment;
	markFor(COMPONENT_REQUIREMENT_UPDATE);
}

g_text_alignment label_t::getAlignment()
{
	return alignment;
}

g_color_argb label_t::getColor()
{
	return color;
}

void label_t::setColor(g_color_argb color)
{
	this->color = color;
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

bool label_t::setNumericProperty(int property, uint32_t value)
{
	if(property == G_UI_PROPERTY_COLOR)
	{
		this->setColor(value);
		return true;
	}

	if(property == G_UI_PROPERTY_ALIGNMENT)
	{
		this->setAlignment((g_text_alignment) value);
		return true;
	}

	return component_t::setNumericProperty(property, value);
}

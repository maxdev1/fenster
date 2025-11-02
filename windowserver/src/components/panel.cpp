// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/panel.hpp"
#include <libwindow/properties.hpp>

void panel_t::paint()
{
	if(!hasGraphics())
		return;

	auto cr = graphics.acquireContext();
	if(!cr)
		return;

	cairo_set_source_rgba(cr, ARGB_FR_FROM(background), ARGB_FG_FROM(background), ARGB_FB_FROM(background),
	                      ARGB_FA_FROM(background));
	cairo_rectangle(cr, 0, 0, getBounds().width, getBounds().height);
	cairo_fill(cr);

	graphics.releaseContext();
}

void panel_t::setBackground(g_color_argb color)
{
	background = color;
	markFor(COMPONENT_REQUIREMENT_PAINT);
	recheckGraphics();
}

g_color_argb panel_t::getBackground()
{
	return background;
}

bool panel_t::getNumericProperty(int property, uint32_t* out)
{
	if(property == G_UI_PROPERTY_BACKGROUND)
	{
		*out = background;
		return true;
	}

	return component_t::getNumericProperty(property, out);
}

bool panel_t::setNumericProperty(int property, uint32_t value)
{
	if(property == G_UI_PROPERTY_BACKGROUND)
	{
		setBackground(value);
		return true;
	}
	return component_t::setNumericProperty(property, value);
}

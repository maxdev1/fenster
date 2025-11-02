// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/panel.hpp"
#include <libwindow/properties.hpp>

namespace fensterserver
{
	void Panel::paint()
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

	void Panel::setBackground(   fenster::ColorArgb color)
	{
		background = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		recheckGraphics();
	}

	   fenster::ColorArgb Panel::getBackground()
	{
		return background;
	}

	bool Panel::getNumericProperty(int property, uint32_t* out)
	{
		if(property == FENSTER_UI_PROPERTY_BACKGROUND)
		{
			*out = background;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool Panel::setNumericProperty(int property, uint32_t value)
	{
		if(property == FENSTER_UI_PROPERTY_BACKGROUND)
		{
			setBackground(value);
			return true;
		}
		return Component::setNumericProperty(property, value);
	}
}

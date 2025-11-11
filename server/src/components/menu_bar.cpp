// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/menu_bar.hpp"

#include <layout/stack_layout.hpp>
#include <libfenster/properties.hpp>

#include "radio_button.hpp"

namespace fensterserver
{
	MenuBar::MenuBar():
		background(ARGB(255, 250, 252, 255))
	{
		auto layout = new StackLayout();
		layout->setOrientation(fenster::Orientation::Horizontal);
		Component::setLayout(layout);
	}

	void MenuBar::paint()
	{
		if(!hasGraphics())
			return;

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();

		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(background));
		cairo_rectangle(cr, 0, 0, getBounds().width, getBounds().height);
		cairo_fill(cr);

		graphics.releaseContext();
	}

	void MenuBar::setBackground(fenster::ColorArgb color)
	{
		background = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		recheckGraphics();
	}

	fenster::ColorArgb MenuBar::getBackground()
	{
		return background;
	}

	bool MenuBar::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Background)
		{
			*out = background;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool MenuBar::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Background)
		{
			setBackground(value);
			return true;
		}
		return Component::setNumericProperty(property, value);
	}
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/radio_group.hpp"

#include <layout/stack_layout_manager.hpp>
#include <libfenster/properties.hpp>

#include "radio_button.hpp"

namespace fensterserver
{
	RadioGroup::RadioGroup():
		background(ARGB(0, 0, 0, 0))
	{
		Component::setLayoutManager(new StackLayoutManager(5));
	}

	void RadioGroup::addChild(Component* comp, ComponentChildReferenceType type)
	{
		Component::addChild(comp, type);

		auto radioButton = dynamic_cast<RadioButton*>(comp);
		if(radioButton)
		{
			radioButton->setInternalCheckedHandler([this, radioButton](bool checked, bool triggeredByGroup)
			{
				if(!triggeredByGroup)
				{
					uncheckOthers(radioButton);
				}
			});
		}
	}

	void RadioGroup::uncheckOthers(RadioButton* selectedRadioButton)
	{
		auto children = acquireChildren();
		for(auto& child: children)
		{
			if(child.component != selectedRadioButton)
			{
				auto otherRadio = dynamic_cast<RadioButton*>(child.component);
				if(otherRadio)
				{
					otherRadio->setChecked(false, true);
				}
			}
		}
		releaseChildren();
	}

	void RadioGroup::paint()
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

	void RadioGroup::setBackground(fenster::ColorArgb color)
	{
		background = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		recheckGraphics();
	}

	fenster::ColorArgb RadioGroup::getBackground()
	{
		return background;
	}

	bool RadioGroup::getNumericProperty(int property, uint32_t* out)
	{
		if(property == FENSTER_UI_PROPERTY_BACKGROUND)
		{
			*out = background;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool RadioGroup::setNumericProperty(int property, uint32_t value)
	{
		if(property == FENSTER_UI_PROPERTY_BACKGROUND)
		{
			setBackground(value);
			return true;
		}
		return Component::setNumericProperty(property, value);
	}
}

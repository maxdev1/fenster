// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/checkbox.hpp"
#include "events/mouse_event.hpp"

namespace fensterserver
{
	Checkbox::Checkbox() :
		checked(false), boxSize(CHECKBOX_DEFAULT_BOX_SIZE), boxTextGap(CHECKBOX_DEFAULT_BOX_TEXT_GAP), hovered(false), pressed(false)
	{
		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
	}

	void Checkbox::handleBoundChanged(const fenster::Rectangle& oldBounds)
	{
		fenster::Rectangle unpositioned = getBounds();
		unpositioned.x = boxSize + boxTextGap;
		unpositioned.y = 0;
		this->label.setBounds(unpositioned);
	}

	void Checkbox::layout()
	{
		fenster::Dimension preferredSize = label.getPreferredSize();
		if(preferredSize.height < boxSize + boxTextGap)
		{
			preferredSize.height = boxSize + boxTextGap;
		}
		preferredSize.width += preferredSize.height;
		setPreferredSize(preferredSize);
	}

	void Checkbox::paint()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		auto background = (pressed ? RGB(240, 240, 240) : (hovered ? RGB(245, 245, 255) : RGB(255, 255, 255)));
		cairo_set_source_rgba(cr, G_COLOR_ARGB_TO_FPARAMS(background));
		cairo_rectangle(cr, 0, 0, boxSize, boxSize);
		cairo_fill(cr);

		auto border = ((hovered || pressed) ? RGB(140, 140, 150) : RGB(160, 160, 170));
		cairo_set_source_rgba(cr, G_COLOR_ARGB_TO_FPARAMS(border));
		cairo_rectangle(cr, 0.5, 0.5, boxSize, boxSize);
		cairo_set_line_width(cr, 1.0);
		cairo_stroke(cr);

		if(checked)
		{
			int pad = 3;
			cairo_set_source_rgba(cr, G_COLOR_ARGB_TO_FPARAMS(RGB(25, 125, 255)));
			cairo_rectangle(cr, 1 + pad, 1.5 + pad, boxSize - pad * 2 - 1, boxSize - pad * 2 - 1.5);
			cairo_fill(cr);
		}

		graphics.releaseContext();
	}

	Component* Checkbox::handleMouseEvent(MouseEvent& me)
	{
		if(me.type == FENSTER_MOUSE_EVENT_ENTER)
		{
			hovered = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
		}
		else if(me.type == FENSTER_MOUSE_EVENT_LEAVE)
		{
			hovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);
		}
		else if(me.type == FENSTER_MOUSE_EVENT_PRESS)
		{
			pressed = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
		}
		else if(me.type == FENSTER_MOUSE_EVENT_RELEASE || me.type == FENSTER_MOUSE_EVENT_DRAG_RELEASE)
		{
			pressed = false;

			fenster::Rectangle minbounds = getBounds();
			minbounds.x = 0;
			minbounds.y = 0;
			if(me.type == FENSTER_MOUSE_EVENT_RELEASE && minbounds.contains(me.position))
			{
				checked = !checked;
			}

			markFor(COMPONENT_REQUIREMENT_PAINT);
		}
		return this;
	}
}

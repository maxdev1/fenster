// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/radio_button.hpp"
#include "events/mouse_event.hpp"
#include <cmath>

namespace fensterserver
{
	RadioButton::RadioButton() :
		checked(false), boxSize(FENSTER_RADIOBUTTON_DEFAULT_CIRCLE_SIZE),
		boxTextGap(FENSTER_RADIOBUTTON_DEFAULT_TEXT_GAP), hovered(false), pressed(false)
	{
		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
	}

	void RadioButton::handleBoundChanged(const fenster::Rectangle& oldBounds)
	{
		fenster::Rectangle unpositioned = getBounds();
		unpositioned.x = boxSize + boxTextGap;
		unpositioned.y = 0;
		this->label.setBounds(unpositioned);
	}

	void RadioButton::layout()
	{
		fenster::Dimension preferredSize = label.getPreferredSize();
		if(preferredSize.height < boxSize + boxTextGap)
		{
			preferredSize.height = boxSize + boxTextGap;
		}
		preferredSize.width += preferredSize.height;
		setPreferredSize(preferredSize);
	}

	void RadioButton::paint()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		double cx = boxSize / 2.0 + 1;
		double cy = boxSize / 2.0 + 1;
		double rOuter = (boxSize - 1.0) / 2.0;

		// background circle
		auto background = (pressed ? _RGB(240, 240, 240) : (hovered ? _RGB(245, 245, 255) : _RGB(255, 255, 255)));
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(background));
		cairo_arc(cr, cx, cy, rOuter, 0, 2 * M_PI);
		cairo_fill(cr);

		// border circle
		auto border = ((hovered || pressed) ? _RGB(140, 140, 150) : _RGB(160, 160, 170));
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(border));
		cairo_set_line_width(cr, 1.0);
		cairo_arc(cr, cx, cy, rOuter, 0, 2 * M_PI);
		cairo_stroke(cr);

		// checked state
		if(checked)
		{
			double rInner = rOuter * 0.55;
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(25, 125, 255)));
			cairo_arc(cr, cx, cy, rInner, 0, 2 * M_PI);
			cairo_fill(cr);
		}

		graphics.releaseContext();
	}

	Component* RadioButton::handleMouseEvent(MouseEvent& me)
	{
		if(me.type == FENSTER_MOUSE_EVENT_ENTER)
		{
			hovered = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == FENSTER_MOUSE_EVENT_LEAVE)
		{
			hovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == FENSTER_MOUSE_EVENT_PRESS)
		{
			pressed = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == FENSTER_MOUSE_EVENT_RELEASE || me.type == FENSTER_MOUSE_EVENT_DRAG_RELEASE)
		{
			pressed = false;

			auto bounds = getBounds();
			bounds.x = 0;
			bounds.y = 0;
			if(me.type == FENSTER_MOUSE_EVENT_RELEASE && bounds.contains(me.position))
			{
				setChecked(true, false);
			}
			return this;
		}

		return nullptr;
	}

	void RadioButton::setCheckedInternal(bool value)
	{
		this->checked = value;
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool RadioButton::isChecked()
	{
		return this->checked;
	}

	void RadioButton::setTitleInternal(std::string title)
	{
		this->label.setTitle(title);
	}

	std::string RadioButton::getTitle()
	{
		return this->label.getTitle();
	}

}

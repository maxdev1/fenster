// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/checkbox.hpp"
#include "events/mouse_event.hpp"

namespace fensterserver
{
	Checkbox::Checkbox() :
		checked(false), boxSize(FENSTER_CHECKBOX_DEFAULT_BOX_SIZE), boxTextGap(FENSTER_CHECKBOX_DEFAULT_TEXT_GAP),
		hovered(false), pressed(false)
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

		auto background = (pressed ? _RGB(240, 240, 240) : (hovered ? _RGB(245, 245, 255) : _RGB(255, 255, 255)));
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(background));
		cairo_rectangle(cr, 0, 0, boxSize, boxSize);
		cairo_fill(cr);

		auto border = ((hovered || pressed) ? _RGB(140, 140, 150) : _RGB(160, 160, 170));
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(border));
		cairo_rectangle(cr, 0.5, 0.5, boxSize, boxSize);
		cairo_set_line_width(cr, 1.0);
		cairo_stroke(cr);

		if(checked)
		{
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(25, 125, 255)));
			cairo_set_line_width(cr, 2.2);
			cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

			int pad = 4;

			cairo_move_to(cr, pad, boxSize * 0.55);
			cairo_line_to(cr, boxSize * 0.40, boxSize - pad);
			cairo_line_to(cr, boxSize - pad, pad + 2);

			cairo_stroke(cr);
		}

		graphics.releaseContext();
	}

	Component* Checkbox::handleMouseEvent(MouseEvent& me)
	{
		if(me.type == fenster::MouseEventType::Enter)
		{
			hovered = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Leave)
		{
			hovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Press)
		{
			pressed = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Release || me.type == fenster::MouseEventType::DragRelease)
		{
			pressed = false;

			auto bounds = getBounds();
			bounds.x = 0;
			bounds.y = 0;
			if(me.type == fenster::MouseEventType::Release && bounds.contains(me.position))
			{
				setChecked(!checked, false);
			}
			return this;
		}

		return nullptr;
	}

	void Checkbox::setCheckedInternal(bool value)
	{
		this->checked = value;
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Checkbox::isChecked()
	{
		return this->checked;
	}

	void Checkbox::setTitleInternal(std::string title)
	{
		this->label.setTitle(title);
	}

	std::string Checkbox::getTitle()
	{
		return this->label.getTitle();
	}

}

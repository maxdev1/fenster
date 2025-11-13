// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/scrollbar.hpp"
#include "components/cursor.hpp"
#include "events/mouse_event.hpp"

#include <stdio.h>

namespace fensterserver
{
	void Scrollbar::paint()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();

		fenster::Rectangle knob = calculateKnob();

		int knobPadding = 3;
		cairo_rectangle(cr, knob.x + knobPadding, knob.y + knobPadding, knob.width - 2 * knobPadding,
		                knob.height - 2 * knobPadding);
		cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
		cairo_fill(cr);

		graphics.releaseContext();
	}

	Component* Scrollbar::handleMouseEvent(MouseEvent& me)
	{
		if(me.type == fenster::MouseEventType::Enter)
		{
			Cursor::set("default");
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Press)
		{
			fenster::Rectangle knob = calculateKnob();
			if(knob.contains(me.position))
			{
				if(orientation == ScrollbarOrientation::VERTICAL)
				{
					dragPressPosition = me.position.y;
					dragViewPosition = knob.y;
				}
				else if(orientation == ScrollbarOrientation::HORIZONTAL)
				{
					dragPressPosition = me.position.x;
					dragViewPosition = knob.x;
				}
			}
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Drag)
		{

			int mousePosition;
			if(orientation == ScrollbarOrientation::VERTICAL)
			{
				mousePosition = me.position.y;
			}
			else if(orientation == ScrollbarOrientation::HORIZONTAL)
			{
				mousePosition = me.position.x;
			}

			int knobSpace = getKnobSpace();
			if(knobSpace <= 0)
				knobSpace = 1;

			int viewPosition = dragViewPosition + (mousePosition - dragPressPosition);
			if(viewPosition < 0)
			{
				viewPosition = 0;
			}
			else if(viewPosition > knobSpace)
			{
				viewPosition = knobSpace;
			}

			int hiddenLength = contentLength - viewportLength;
			setModelPosition((int)((float)(viewPosition * hiddenLength) / (float)knobSpace));

			if(scrollHandler)
				scrollHandler->handleScroll(this);
			return this;
		}
		return nullptr;
	}

	void Scrollbar::setViewLengths(int viewportLength, int contentLength)
	{
		this->viewportLength = viewportLength;
		this->contentLength = contentLength;

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void Scrollbar::setModelPosition(int pos)
	{
		modelPosition = pos;

		if(modelPosition < 0)
			modelPosition = 0;
		else if(modelPosition > contentLength - viewportLength)
			modelPosition = contentLength - viewportLength;

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	fenster::Rectangle Scrollbar::calculateKnob()
	{
		fenster::Rectangle bounds = getBounds();
		int knobLength = getKnobLength();
		int knobSpace = getKnobSpace();

		int hiddenLength = contentLength - viewportLength;
		if(hiddenLength <= 0)
			hiddenLength = 1;

		int viewPosition = (knobSpace * modelPosition) / hiddenLength;

		if(orientation == ScrollbarOrientation::VERTICAL)
		{
			return fenster::Rectangle(0, viewPosition, bounds.width, knobLength);
		}
		else if(orientation == ScrollbarOrientation::HORIZONTAL)
		{
			return fenster::Rectangle(viewPosition, 0, knobLength, bounds.height);
		}

		return fenster::Rectangle(0, 0, 1, 1);
	}

	int Scrollbar::getKnobSpace()
	{
		if(orientation == ScrollbarOrientation::VERTICAL)
		{
			return getBounds().height - getKnobLength();
		}
		else if(orientation == ScrollbarOrientation::HORIZONTAL)
		{
			return getBounds().width - getKnobLength();
		}

		return 0;
	}

	int Scrollbar::getKnobLength()
	{
		int bounds;
		if(orientation == ScrollbarOrientation::VERTICAL)
		{
			bounds = getBounds().height;
		}
		else if(orientation == ScrollbarOrientation::HORIZONTAL)
		{
			bounds = getBounds().width;
		}

		int total = contentLength <= 0 ? 1 : contentLength;
		int size = (bounds * viewportLength) / contentLength;
		if(size < 20)
		{
			size = 20;
		}
		return size;
	}
}

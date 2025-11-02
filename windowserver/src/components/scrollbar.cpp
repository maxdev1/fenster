// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/scrollbar.hpp"
#include "components/cursor.hpp"
#include "events/mouse_event.hpp"

#include <stdio.h>

void scrollbar_t::paint()
{
	auto cr = graphics.acquireContext();
	if(!cr)
		return;

	clearSurface();

	g_rectangle knob = calculateKnob();

	int knobPadding = 3;
	cairo_rectangle(cr, knob.x + knobPadding, knob.y + knobPadding, knob.width - 2 * knobPadding,
	                knob.height - 2 * knobPadding);
	cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
	cairo_fill(cr);

	graphics.releaseContext();
}

component_t* scrollbar_t::handleMouseEvent(mouse_event_t& me)
{
	if(me.type == G_MOUSE_EVENT_ENTER)
	{
		cursor_t::set("default");
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}
	else if(me.type == G_MOUSE_EVENT_PRESS)
	{
		g_rectangle knob = calculateKnob();
		if(knob.contains(me.position))
		{
			if(orientation == scrollbar_orientation_t::VERTICAL)
			{
				dragPressPosition = me.position.y;
				dragViewPosition = knob.y;
			}
			else if(orientation == scrollbar_orientation_t::HORIZONTAL)
			{
				dragPressPosition = me.position.x;
				dragViewPosition = knob.x;
			}
		}
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}
	else if(me.type == G_MOUSE_EVENT_DRAG)
	{

		int mousePosition;
		if(orientation == scrollbar_orientation_t::VERTICAL)
		{
			mousePosition = me.position.y;
		}
		else if(orientation == scrollbar_orientation_t::HORIZONTAL)
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
		setModelPosition((viewPosition * hiddenLength) / knobSpace);

		if(scrollHandler)
			scrollHandler->handleScroll(this);
	}
	return this;
}

void scrollbar_t::setViewLengths(int viewportLength, int contentLength)
{
	this->viewportLength = viewportLength;
	this->contentLength = contentLength;

	markFor(COMPONENT_REQUIREMENT_PAINT);
}

void scrollbar_t::setModelPosition(int pos)
{
	modelPosition = pos;

	if(modelPosition < 0)
		modelPosition = 0;
	else if(modelPosition > contentLength - viewportLength)
		modelPosition = contentLength - viewportLength;

	markFor(COMPONENT_REQUIREMENT_PAINT);
}

g_rectangle scrollbar_t::calculateKnob()
{
	g_rectangle bounds = getBounds();
	int knobLength = getKnobLength();
	int knobSpace = getKnobSpace();

	int hiddenLength = contentLength - viewportLength;
	if(hiddenLength <= 0)
		hiddenLength = 1;

	int viewPosition = (knobSpace * modelPosition) / hiddenLength;

	if(orientation == scrollbar_orientation_t::VERTICAL)
	{
		return g_rectangle(0, viewPosition, bounds.width, knobLength);
	}
	else if(orientation == scrollbar_orientation_t::HORIZONTAL)
	{
		return g_rectangle(viewPosition, 0, knobLength, bounds.height);
	}

	return g_rectangle(0, 0, 1, 1);
}

int scrollbar_t::getKnobSpace()
{
	if(orientation == scrollbar_orientation_t::VERTICAL)
	{
		return getBounds().height - getKnobLength();
	}
	else if(orientation == scrollbar_orientation_t::HORIZONTAL)
	{
		return getBounds().width - getKnobLength();
	}

	return 0;
}

int scrollbar_t::getKnobLength()
{
	int bounds;
	if(orientation == scrollbar_orientation_t::VERTICAL)
	{
		bounds = getBounds().height;
	}
	else if(orientation == scrollbar_orientation_t::HORIZONTAL)
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

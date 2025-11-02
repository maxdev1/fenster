// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "events/event_processor.hpp"
#include "component_registry.hpp"
#include "components/cursor.hpp"
#include "components/text/text_field.hpp"
#include "components/window.hpp"
#include "events/event.hpp"
#include "events/key_event.hpp"
#include "events/mouse_event.hpp"
#include "windowserver.hpp"

event_processor_t::event_processor_t()
{
	multiclickTimespan = DEFAULT_MULTICLICK_TIMESPAN;
}

void event_processor_t::bufferKeyEvent(g_key_info keyInfo)
{
	platformAcquireMutex(key_info_buffer_lock);
	key_info_buffer.push_back(keyInfo);
	platformReleaseMutex(key_info_buffer_lock);
}

void event_processor_t::process()
{
	processMouseState();
	processKeyState();
}

void event_processor_t::processKeyState()
{
	platformAcquireMutex(key_info_buffer_lock);
	while(key_info_buffer.size() > 0)
	{
		translateKeyEvent(key_info_buffer.back());
		key_info_buffer.pop_back();
	}
	platformReleaseMutex(key_info_buffer_lock);
}

void event_processor_t::translateKeyEvent(g_key_info& info)
{
	if(cursor_t::focusedComponent != -1)
	{
		auto focusedComponent = component_registry_t::get(cursor_t::focusedComponent);

		if(focusedComponent)
		{
			key_event_t k;
			k.info = info;
			windowserver_t::instance()->dispatch(focusedComponent, k);
		}
	}
}

void event_processor_t::processMouseState()
{
	g_point previousPosition = cursor_t::position;

	g_mouse_button previousPressedButtons = cursor_t::pressedButtons;

	windowserver_t* instance = windowserver_t::instance();
	screen_t* screen = instance->screen;

	if(cursor_t::position != cursor_t::nextPosition)
	{
		screen->markDirty(cursor_t::getArea());
		cursor_t::position = cursor_t::nextPosition;
		screen->markDirty(cursor_t::getArea());
	}

	// set pressed buttons
	cursor_t::pressedButtons = cursor_t::nextPressedButtons;

	mouse_event_t baseEvent;
	baseEvent.screenPosition = cursor_t::position;
	baseEvent.position = baseEvent.screenPosition;
	baseEvent.buttons = cursor_t::pressedButtons;
	baseEvent.scroll = cursor_t::nextScroll;
	cursor_t::nextScroll = 0;

	// Press
	if((!(previousPressedButtons & G_MOUSE_BUTTON_1) && (cursor_t::pressedButtons & G_MOUSE_BUTTON_1)) ||
	   (!(previousPressedButtons & G_MOUSE_BUTTON_2) && (cursor_t::pressedButtons & G_MOUSE_BUTTON_2)) ||
	   (!(previousPressedButtons & G_MOUSE_BUTTON_3) && (cursor_t::pressedButtons & G_MOUSE_BUTTON_3)))
	{
		// Prepare event
		mouse_event_t pressEvent = baseEvent;
		pressEvent.type = G_MOUSE_EVENT_PRESS;

		// Multiclicks
		static uint64_t lastClick = 0;
		static int clickCount = 0;
		uint64_t currentClick = platformMillis();
		uint64_t diff = currentClick - lastClick;
		if(diff < multiclickTimespan)
		{
			++clickCount;
		}
		else
		{
			clickCount = 1;
		}
		lastClick = currentClick;
		pressEvent.clickCount = clickCount;

		// Send event
		auto pressedComponent = instance->dispatch(screen, pressEvent);

		if(pressedComponent)
		{
			// Prepare drag
			cursor_t::pressedComponent = pressedComponent->id;
			cursor_t::draggedComponent = cursor_t::pressedComponent;

			// Switch focus
			if(cursor_t::pressedComponent != cursor_t::focusedComponent)
			{
				windowserver_t::instance()->switchFocus(pressedComponent);
			}
		}
		else
		{
			cursor_t::pressedComponent = -1;
		}

		// Release
	}
	else if(
		((previousPressedButtons & G_MOUSE_BUTTON_1) && !(cursor_t::pressedButtons & G_MOUSE_BUTTON_1)) ||
		((previousPressedButtons & G_MOUSE_BUTTON_2) && !(cursor_t::pressedButtons & G_MOUSE_BUTTON_2)) ||
		((previousPressedButtons & G_MOUSE_BUTTON_3) && !(cursor_t::pressedButtons & G_MOUSE_BUTTON_3)))
	{

		if(cursor_t::draggedComponent != -1)
		{
			auto draggedComponent = component_registry_t::get(cursor_t::draggedComponent);

			if(draggedComponent)
			{
				mouse_event_t releaseDraggedEvent = baseEvent;
				releaseDraggedEvent.type = G_MOUSE_EVENT_DRAG_RELEASE;
				instance->dispatchUpwards(draggedComponent, releaseDraggedEvent);
			}

			cursor_t::draggedComponent = -1;
		}

		if(cursor_t::pressedComponent != -1)
		{
			auto pressedComponent = component_registry_t::get(cursor_t::pressedComponent);
			if(pressedComponent)
			{
				mouse_event_t releaseEvent = baseEvent;
				releaseEvent.type = G_MOUSE_EVENT_RELEASE;
				instance->dispatchUpwards(pressedComponent, releaseEvent);
			}

			cursor_t::pressedComponent = -1;
		}

		// Move or drag
	}
	else if(cursor_t::position != previousPosition || baseEvent.scroll != 0)
	{
		if(cursor_t::draggedComponent != -1)
		{
			auto draggedComponent = component_registry_t::get(cursor_t::draggedComponent);

			if(draggedComponent)
			{
				// Dragging
				mouse_event_t dragEvent = baseEvent;
				dragEvent.type = G_MOUSE_EVENT_DRAG;
				instance->dispatchUpwards(draggedComponent, dragEvent);
			}
		}
		else
		{
			// Moving
			mouse_event_t moveEvent = baseEvent;
			moveEvent.type = G_MOUSE_EVENT_MOVE;

			component_t* newHoveredComponent = instance->dispatch(screen, moveEvent);

			// Scrolling
			if(baseEvent.scroll != 0 && newHoveredComponent)
			{
				mouse_event_t scrollEvent = baseEvent;
				scrollEvent.type = G_MOUSE_EVENT_SCROLL;
				instance->dispatchUpwards(newHoveredComponent, scrollEvent);
			}

			// Post enter or leave events
			if(newHoveredComponent && (newHoveredComponent->id != cursor_t::hoveredComponent) &&
			   (cursor_t::draggedComponent == -1 || cursor_t::draggedComponent != cursor_t::hoveredComponent))
			{
				// Leave
				if(cursor_t::hoveredComponent != -1)
				{
					auto hoveredComponent = component_registry_t::get(cursor_t::hoveredComponent);

					if(hoveredComponent)
					{
						mouse_event_t leaveEvent = baseEvent;
						leaveEvent.type = G_MOUSE_EVENT_LEAVE;
						instance->dispatchUpwards(hoveredComponent, leaveEvent);
						cursor_t::hoveredComponent = -1;
					}
				}

				if(newHoveredComponent)
				{
					// Enter
					mouse_event_t enterEvent = baseEvent;
					enterEvent.type = G_MOUSE_EVENT_ENTER;
					cursor_t::hoveredComponent = newHoveredComponent->id;
					instance->dispatchUpwards(newHoveredComponent, enterEvent);
				}
			}
		}
	}
}

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
#include "server.hpp"

namespace fensterserver
{
	EventProcessor::EventProcessor()
	{
		multiclickTimespan = DEFAULT_MULTICLICK_TIMESPAN;
	}

	void EventProcessor::bufferKeyEvent(fenster::KeyInfo keyInfo)
	{
		fenster::platformAcquireMutex(key_info_buffer_lock);
		key_info_buffer.push_back(keyInfo);
		fenster::platformReleaseMutex(key_info_buffer_lock);
	}

	void EventProcessor::process()
	{
		processMouseState();
		processKeyState();
	}

	void EventProcessor::processKeyState()
	{
		fenster::platformAcquireMutex(key_info_buffer_lock);
		while(key_info_buffer.size() > 0)
		{
			translateKeyEvent(key_info_buffer.back());
			key_info_buffer.pop_back();
		}
		fenster::platformReleaseMutex(key_info_buffer_lock);
	}

	void EventProcessor::translateKeyEvent(fenster::KeyInfo& info)
	{
		if(Cursor::focusedComponent != -1)
		{
			auto focusedComponent = ComponentRegistry::get(Cursor::focusedComponent);

			if(focusedComponent)
			{
				KeyEvent k;
				k.info = info;
				Server::instance()->dispatch(focusedComponent, k);
			}
		}
	}

	void EventProcessor::processMouseState()
	{
		fenster::Point previousPosition = Cursor::position;

		fenster::MouseButton previousPressedButtons = Cursor::pressedButtons;

		Server* instance = Server::instance();
		Screen* screen = instance->screen;

		if(Cursor::position != Cursor::nextPosition)
		{
			screen->markDirty(Cursor::getArea());
			Cursor::position = Cursor::nextPosition;
			screen->markDirty(Cursor::getArea());
		}

		// set pressed buttons
		Cursor::pressedButtons = Cursor::nextPressedButtons;

		MouseEvent baseEvent;
		baseEvent.screenPosition = Cursor::position;
		baseEvent.position = baseEvent.screenPosition;
		baseEvent.buttons = Cursor::pressedButtons;
		baseEvent.scroll = Cursor::nextScroll;
		Cursor::nextScroll = 0;

		// Press
		if((!(previousPressedButtons & FENSTER_MOUSE_BUTTON_1) && (Cursor::pressedButtons & FENSTER_MOUSE_BUTTON_1)) ||
		   (!(previousPressedButtons & FENSTER_MOUSE_BUTTON_2) && (Cursor::pressedButtons & FENSTER_MOUSE_BUTTON_2)) ||
		   (!(previousPressedButtons & FENSTER_MOUSE_BUTTON_3) && (Cursor::pressedButtons & FENSTER_MOUSE_BUTTON_3)))
		{
			// Prepare event
			MouseEvent pressEvent = baseEvent;
			pressEvent.type = FENSTER_MOUSE_EVENT_PRESS;

			// Multiclicks
			static uint64_t lastClick = 0;
			static int clickCount = 0;
			uint64_t currentClick = fenster::platformMillis();
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
				Cursor::pressedComponent = pressedComponent->id;
				Cursor::draggedComponent = Cursor::pressedComponent;

				// Switch focus
				if(Cursor::pressedComponent != Cursor::focusedComponent)
				{
					Server::instance()->switchFocus(pressedComponent);
				}
			}
			else
			{
				Cursor::pressedComponent = -1;
			}

			// Release
		}
		else if(
			((previousPressedButtons & FENSTER_MOUSE_BUTTON_1) && !(Cursor::pressedButtons & FENSTER_MOUSE_BUTTON_1)) ||
			((previousPressedButtons & FENSTER_MOUSE_BUTTON_2) && !(Cursor::pressedButtons & FENSTER_MOUSE_BUTTON_2)) ||
			((previousPressedButtons & FENSTER_MOUSE_BUTTON_3) && !(Cursor::pressedButtons & FENSTER_MOUSE_BUTTON_3)))
		{

			if(Cursor::draggedComponent != -1)
			{
				auto draggedComponent = ComponentRegistry::get(Cursor::draggedComponent);

				if(draggedComponent)
				{
					MouseEvent releaseDraggedEvent = baseEvent;
					releaseDraggedEvent.type = FENSTER_MOUSE_EVENT_DRAG_RELEASE;
					instance->dispatchUpwards(draggedComponent, releaseDraggedEvent);
				}

				Cursor::draggedComponent = -1;
			}

			if(Cursor::pressedComponent != -1)
			{
				auto pressedComponent = ComponentRegistry::get(Cursor::pressedComponent);
				if(pressedComponent)
				{
					MouseEvent releaseEvent = baseEvent;
					releaseEvent.type = FENSTER_MOUSE_EVENT_RELEASE;
					instance->dispatchUpwards(pressedComponent, releaseEvent);
				}

				Cursor::pressedComponent = -1;
			}

			// Move or drag
		}
		else if(Cursor::position != previousPosition || baseEvent.scroll != 0)
		{
			if(Cursor::draggedComponent != -1)
			{
				auto draggedComponent = ComponentRegistry::get(Cursor::draggedComponent);

				if(draggedComponent)
				{
					// Dragging
					MouseEvent dragEvent = baseEvent;
					dragEvent.type = FENSTER_MOUSE_EVENT_DRAG;
					instance->dispatchUpwards(draggedComponent, dragEvent);
				}
			}
			else
			{
				// Moving
				MouseEvent moveEvent = baseEvent;
				moveEvent.type = FENSTER_MOUSE_EVENT_MOVE;

				Component* newHoveredComponent = instance->dispatch(screen, moveEvent);

				// Scrolling
				if(baseEvent.scroll != 0 && newHoveredComponent)
				{
					MouseEvent scrollEvent = baseEvent;
					scrollEvent.type = FENSTER_MOUSE_EVENT_SCROLL;
					instance->dispatchUpwards(newHoveredComponent, scrollEvent);
				}

				// Post enter or leave events
				if(newHoveredComponent && (newHoveredComponent->id != Cursor::hoveredComponent) &&
				   (Cursor::draggedComponent == -1 || Cursor::draggedComponent != Cursor::hoveredComponent))
				{
					// Leave
					if(Cursor::hoveredComponent != -1)
					{
						auto hoveredComponent = ComponentRegistry::get(Cursor::hoveredComponent);

						if(hoveredComponent)
						{
							MouseEvent leaveEvent = baseEvent;
							leaveEvent.type = FENSTER_MOUSE_EVENT_LEAVE;
							instance->dispatchUpwards(hoveredComponent, leaveEvent);
							Cursor::hoveredComponent = -1;
						}
					}

					if(newHoveredComponent)
					{
						// Enter
						MouseEvent enterEvent = baseEvent;
						enterEvent.type = FENSTER_MOUSE_EVENT_ENTER;
						Cursor::hoveredComponent = newHoveredComponent->id;
						instance->dispatchUpwards(newHoveredComponent, enterEvent);
					}
				}
			}
		}
	}
}

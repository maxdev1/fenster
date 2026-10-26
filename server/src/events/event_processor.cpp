// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "events/event_processor.hpp"
#include "component_registry.hpp"
#include "components/cursor.hpp"
#include "components/window.hpp"
#include "events/event.hpp"
#include "events/key_event.hpp"
#include "events/mouse_event.hpp"
#include "server.hpp"
#include "input/mouse_grab.hpp"

namespace fensterserver
{
	EventProcessor::EventProcessor()
	{
		multiclickTimespan = DEFAULT_MULTICLICK_TIMESPAN;
	}

	void EventProcessor::bufferKeyEvent(fenster::KeyInfo keyInfo)
	{
		const bool consumed = MouseGrab::consumeKey(keyInfo);
		fenster::platformAcquireMutex(key_info_buffer_lock);
		ctrlDown = keyInfo.ctrl;
		if(!consumed) key_info_buffer.push_back(keyInfo);
		fenster::platformReleaseMutex(key_info_buffer_lock);
	}

	void EventProcessor::bufferMouseState(int scroll, uint64_t arrivalMillis, uint64_t generation)
	{
		MouseSample sample;
		fenster::platformAcquireMutex(Cursor::inputLock);
		sample.position = Cursor::nextPosition;
		sample.buttons = Cursor::nextPressedButtons;
		fenster::platformReleaseMutex(Cursor::inputLock);
		sample.scroll = scroll;
		fenster::platformAcquireMutex(key_info_buffer_lock);
		sample.ctrl = ctrlDown;
		fenster::platformReleaseMutex(key_info_buffer_lock);
		sample.arrivalMillis = arrivalMillis;
		sample.generation = generation ? generation : MouseGrab::generation();

		fenster::platformAcquireMutex(mouseBufferLock);
		sample.buttonChange = sample.buttons != lastBufferedButtons;
		// Motion may replace motion, but never the position of a button edge or
		// a scroll sample. Preserve those even while component painting is busy.
		if(!sample.buttonChange && scroll == 0 && !mouseBuffer.empty() &&
		   !mouseBuffer.back().buttonChange && mouseBuffer.back().scroll == 0 &&
		   mouseBuffer.back().buttons == sample.buttons && mouseBuffer.back().generation == sample.generation)
			mouseBuffer.back() = sample;
		else
			mouseBuffer.push_back(sample);
		lastBufferedButtons = sample.buttons;
		fenster::platformReleaseMutex(mouseBufferLock);
	}

	void EventProcessor::process()
	{
		processMouseState();
		processKeyState();
	}

	void EventProcessor::processKeyState()
	{
		std::deque<fenster::KeyInfo> pending;
		fenster::platformAcquireMutex(key_info_buffer_lock);
		pending.swap(key_info_buffer);
		fenster::platformReleaseMutex(key_info_buffer_lock);

		// Preserve arrival order and let input continue while callbacks run.
		while(!pending.empty())
		{
			translateKeyEvent(pending.front());
			pending.pop_front();
		}
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

	void EventProcessor::handlePress(Server* instance, Screen* screen, MouseEvent& event, uint64_t arrivalMillis)
	{
		event.type = fenster::MouseEventType::Press;

		// Use input time: delayed UI dispatch must not turn a double click into
		// two single clicks or unrelated presses into a double click.
		uint64_t diff = arrivalMillis - lastClick;
		if(clickCount > 0 && diff < multiclickTimespan)
		{
			++clickCount;
		}
		else
		{
			clickCount = 1;
		}
		lastClick = arrivalMillis;
		event.clickCount = clickCount;

		// Send event
		auto pressedComponent = instance->dispatch(screen, event);
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
	}

	void EventProcessor::handleRelease(Server* instance, MouseEvent& event, bool cancelled)
	{
		// Deliver the real release before ending capture; cancellation only ends capture.
		if(!cancelled && Cursor::pressedComponent != -1) {
			if(auto pressed = ComponentRegistry::get(Cursor::pressedComponent)) {
				MouseEvent release = event; release.type = fenster::MouseEventType::Release;
				instance->dispatchUpwards(pressed, release);
			}
		}
		if(Cursor::draggedComponent != -1) {
			if(auto dragged = ComponentRegistry::get(Cursor::draggedComponent)) {
				MouseEvent release = event; release.type = fenster::MouseEventType::DragRelease;
				instance->dispatchUpwards(dragged, release);
			}
		}
		Cursor::draggedComponent = Cursor::pressedComponent = -1;
	}

	void EventProcessor::handleMoveOrDrag(Server* instance, Screen* screen, MouseEvent& event)
	{
		if(Cursor::draggedComponent != -1)
		{
			auto draggedComponent = ComponentRegistry::get(Cursor::draggedComponent);

			if(draggedComponent && draggedComponent->canHandleEvents())
			{
				// Dragging
				MouseEvent dragEvent = event;
				dragEvent.type = fenster::MouseEventType::Drag;
				instance->dispatchUpwards(draggedComponent, dragEvent);
				return;
			}
			handleRelease(instance, event, true);
		}

		// Moving
		MouseEvent moveEvent = event;
		moveEvent.type = fenster::MouseEventType::Move;

		Component* newHoveredComponent = instance->dispatch(screen, moveEvent);

		// Scrolling
		if(event.scroll != 0)
		{
			MouseEvent scrollEvent = event;
			scrollEvent.type = fenster::MouseEventType::Scroll;
			// Hit-test from the screen so a native scroll pane handles the wheel
			// before a descendant's generic client mouse listener can consume it.
			instance->dispatch(screen, scrollEvent);
		}

		// Post enter or leave events
		if((newHoveredComponent ? newHoveredComponent->id : -1) != Cursor::hoveredComponent)
		{
			// Leave
			if(Cursor::hoveredComponent != -1)
			{
				auto hoveredComponent = ComponentRegistry::get(Cursor::hoveredComponent);

				if(hoveredComponent)
				{
					MouseEvent leaveEvent = event;
					leaveEvent.type = fenster::MouseEventType::Leave;
					instance->dispatchUpwards(hoveredComponent, leaveEvent);
				}
			}

			Cursor::hoveredComponent = -1;
			if(newHoveredComponent)
			{
				// Enter
				MouseEvent enterEvent = event;
				enterEvent.type = fenster::MouseEventType::Enter;
				Cursor::hoveredComponent = newHoveredComponent->id;
				instance->dispatchUpwards(newHoveredComponent, enterEvent);
			}
		}

		// Resolve after Leave/Enter callbacks so the old target cannot overwrite
		// the new shape. Plain components and empty space always use the default.
		Cursor::set(newHoveredComponent
			? newHoveredComponent->getCursorAt(event.screenPosition - newHoveredComponent->getLocationOnScreen())
			: "default");
	}

	void EventProcessor::refreshHover()
	{
		bool grabbed;
		MouseGrab::generation(&grabbed);
		if(grabbed) return;
		if(auto dragged = ComponentRegistry::get(Cursor::draggedComponent))
			if(dragged->canHandleEvents()) return;
		MouseEvent event; event.synthetic = true;
		event.screenPosition = event.position = Cursor::position;
		event.buttons = Cursor::pressedButtons;
		event.scroll = 0;
		auto instance = Server::instance();
		handleMoveOrDrag(instance, instance->screen, event);
	}

	void EventProcessor::processMouseState()
	{
		std::deque<MouseSample> pending;
		fenster::platformAcquireMutex(mouseBufferLock);
		pending.swap(mouseBuffer);
		fenster::platformReleaseMutex(mouseBufferLock);

		Server* instance = Server::instance();
		Screen* screen = instance->screen;
		auto resetHandoff = [&](uint64_t generation)
		{
			if(mouseGeneration == generation)
				return;
			mouseGeneration = generation;
			// Finish any desktop drag before switching routing modes.
			MouseEvent cancel;
			cancel.screenPosition = cancel.position = Cursor::position;
			cancel.buttons = fenster::MouseButton::None;
			cancel.scroll = 0;
			handleRelease(instance, cancel, true);
			if(auto oldHover = ComponentRegistry::get(Cursor::hoveredComponent)) {
				cancel.type = fenster::MouseEventType::Leave;
				instance->dispatchUpwards(oldHover, cancel);
			}
			Cursor::pressedComponent = Cursor::draggedComponent = Cursor::hoveredComponent = -1;
			Cursor::pressedButtons = fenster::MouseButton::None;
			clickCount = 0;
		};
		resetHandoff(MouseGrab::generation());
		for(const auto& sample : pending)
		{
			bool grabbed;
			auto generation = MouseGrab::generation(&grabbed);
			resetHandoff(generation);
			if(grabbed || sample.generation != generation)
				continue;
			const auto previousPosition = Cursor::position;
			auto previousPressedButtons = Cursor::pressedButtons;
			Cursor::position = sample.position;
			Cursor::pressedButtons = sample.buttons;

			MouseEvent baseEvent;
			baseEvent.screenPosition = sample.position;
			baseEvent.position = sample.position;
			baseEvent.buttons = sample.buttons;
			baseEvent.scroll = sample.scroll;
			baseEvent.ctrl = sample.ctrl;

			const bool pressed =
				(!(previousPressedButtons & fenster::MouseButton::Button1) && (Cursor::pressedButtons & fenster::MouseButton::Button1)) ||
				(!(previousPressedButtons & fenster::MouseButton::Button2) && (Cursor::pressedButtons & fenster::MouseButton::Button2)) ||
				(!(previousPressedButtons & fenster::MouseButton::Button3) && (Cursor::pressedButtons & fenster::MouseButton::Button3));
			const bool released =
				((previousPressedButtons & fenster::MouseButton::Button1) && !(Cursor::pressedButtons & fenster::MouseButton::Button1)) ||
				((previousPressedButtons & fenster::MouseButton::Button2) && !(Cursor::pressedButtons & fenster::MouseButton::Button2)) ||
				((previousPressedButtons & fenster::MouseButton::Button3) && !(Cursor::pressedButtons & fenster::MouseButton::Button3));

			if(released)
				handleRelease(instance, baseEvent);
			if(pressed && Cursor::draggedComponent == -1) handleMoveOrDrag(instance, screen, baseEvent);
			if(pressed)
				handlePress(instance, screen, baseEvent, sample.arrivalMillis);
			if(released || (!pressed && sample.position != previousPosition) || sample.scroll != 0)
				handleMoveOrDrag(instance, screen, baseEvent);
		}
	}
}

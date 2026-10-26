// SPDX-License-Identifier: LGPL-3.0-or-later
#include "input/mouse_grab.hpp"
#include "component_registry.hpp"
#include "process_registry.hpp"
#include "components/cursor.hpp"
#include <algorithm>
#include <deque>

namespace fensterserver
{
	namespace
	{
		SYS_MUTEX_T grabLock = fenster::platformInitializeMutex(true);
		fenster::ComponentId owner = -1;
		SYS_TID_T ownerPid = SYS_TID_NONE, receiver = SYS_TID_NONE;
		uint64_t epoch = 1;
		uint8_t physicalButtons = 0, blockedButtons = 0;
		bool suppressedEscape = false;
		struct PendingRelease { fenster::ComponentId id; SYS_TID_T pid, receiver; };
		std::deque<PendingRelease> pendingReleases;
		struct Lock
		{
			Lock() { fenster::platformAcquireMutex(grabLock); }
			~Lock() { fenster::platformReleaseMutex(grabLock); }
		};

		bool sendTo(SYS_TID_T target, void* event, size_t size)
		{
#ifdef _GHOST_
			// A stalled client must never block the input receiver or escape chord.
			return g_send_message_m(target, event, size, G_MESSAGE_SEND_MODE_NON_BLOCKING) == G_MESSAGE_SEND_STATUS_SUCCESSFUL;
#else
			return fenster::platformSendMessage(target, event, size, SYS_TX_NONE);
#endif
		}

		void release(bool notify = true)
		{
			if(owner == -1)
				return;
			fenster::ComponentMouseGrabEvent event{};
			event.header.type = fenster::ComponentEventType::MouseGrab;
			event.header.component_id = owner;
			event.grabbed = false;
			if(notify && !sendTo(receiver, &event, sizeof(event)))
				pendingReleases.push_back({owner, ownerPid, receiver});
			owner = -1;
			ownerPid = receiver = SYS_TID_NONE;
			blockedButtons = physicalButtons;
			++epoch;
			Cursor::setHidden(false);
		}
	}

	bool MouseGrab::set(fenster::ComponentId id, SYS_TID_T pid, bool grabbed)
	{
		if(!ComponentRegistry::owns(pid, id))
			return false;
		auto component = ComponentRegistry::get(id);
		if(!component)
			return false;
		Lock lock;
		for(const auto& pending : pendingReleases)
			if(pending.id == id && grabbed)
				return false; // Deliver the old release before a fresh acquisition.
		if(!grabbed)
		{
			if(owner != -1 && (owner != id || ownerPid != pid))
				return false;
			release();
			return true;
		}
#if !defined(_GHOST_) && !defined(FENSTER_MOUSE_GRAB_TEST)
		// The native simulation backends do not provide raw device motion yet.
		return false;
#endif
		if(owner != -1)
			return owner == id && ownerPid == pid;
		if(!component->canHandleEvents() || Cursor::focusedComponent != id)
			return false;
		receiver = ProcessRegistry::get(pid);
		if(receiver == SYS_TID_NONE)
			return false;
		fenster::ComponentMouseGrabEvent event{};
		event.header.type = fenster::ComponentEventType::MouseGrab;
		event.header.component_id = id;
		event.grabbed = true;
		if(!sendTo(receiver, &event, sizeof(event)))
		{
			receiver = SYS_TID_NONE;
			return false;
		}
		owner = id;
		ownerPid = pid;
		blockedButtons = physicalButtons;
		++epoch;
		Cursor::setHidden(true);
		return true;
	}

	uint64_t MouseGrab::generation(bool* grabbed)
	{
		Lock lock;
		if(grabbed)
			*grabbed = owner != -1;
		return epoch;
	}

	bool MouseGrab::routeMouse(int dx, int dy, fenster::MouseButton& buttons, int scroll, uint64_t& generation)
	{
		Lock lock;
		physicalButtons = static_cast<uint8_t>(buttons);
		blockedButtons &= physicalButtons;
		buttons = static_cast<fenster::MouseButton>(physicalButtons & ~blockedButtons);
		generation = epoch;
		if(owner == -1)
			return false;
		fenster::ComponentRelativeMouseEvent event{};
		event.header.type = fenster::ComponentEventType::RelativeMouse;
		event.header.component_id = owner;
		event.movement = {dx, dy};
		event.buttons = buttons;
		event.scroll = scroll;
		if(!sendTo(receiver, &event, sizeof(event)))
			release();
		return true;
	}

	bool MouseGrab::consumeKey(const fenster::KeyInfo& key)
	{
		if(key.key != "KEY_ESC")
			return false;
		Lock lock;
		if(suppressedEscape)
		{
			if(!key.pressed)
				suppressedEscape = false;
			return true;
		}
		if(key.pressed && key.shift && owner != -1)
		{
			suppressedEscape = true;
			release();
			return true;
		}
		return false;
	}

	void MouseGrab::releaseComponent(fenster::ComponentId id)
	{
		Lock lock;
		if(owner == id)
			release();
	}

	void MouseGrab::releaseProcess(SYS_TID_T pid)
	{
		Lock lock;
		if(ownerPid == pid)
			release(false);
		pendingReleases.erase(std::remove_if(pendingReleases.begin(), pendingReleases.end(),
			[pid](const PendingRelease& pending) { return pending.pid == pid; }), pendingReleases.end());
	}

	void MouseGrab::retryNotifications()
	{
		Lock lock;
		for(auto pending = pendingReleases.begin(); pending != pendingReleases.end();)
		{
			fenster::ComponentMouseGrabEvent event{};
			event.header.type = fenster::ComponentEventType::MouseGrab;
			event.header.component_id = pending->id;
			event.grabbed = false;
			if(sendTo(pending->receiver, &event, sizeof(event)))
				pending = pendingReleases.erase(pending);
			else
				++pending;
		}
	}

	void MouseGrab::focusChanged(fenster::ComponentId id)
	{
		Lock lock;
		if(owner != id)
			release();
	}

	void MouseGrab::releaseFor(Component* component)
	{
		fenster::ComponentId grabbed;
		{ Lock lock; grabbed = owner; }
		auto target = ComponentRegistry::get(grabbed);
		if(target && (target == component || target->isChildOf(component)))
			releaseComponent(grabbed);
	}
}

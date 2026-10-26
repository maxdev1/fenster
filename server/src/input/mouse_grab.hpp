// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once
#include <libfenster/interface.hpp>

namespace fensterserver
{
	class Component;
	class MouseGrab
	{
	public:
		static bool set(fenster::ComponentId id, SYS_TID_T pid, bool grabbed);
		// Called before desktop clamping. Filters buttons held across a handoff.
		static bool routeMouse(int dx, int dy, fenster::MouseButton& buttons, int scroll, uint64_t& generation);
		static bool consumeKey(const fenster::KeyInfo& key);
		static void retryNotifications();
		static uint64_t generation(bool* grabbed = nullptr);
		static void releaseFor(Component* component);
		static void releaseComponent(fenster::ComponentId id);
		static void releaseProcess(SYS_TID_T pid);
		static void focusChanged(fenster::ComponentId id);
	};
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _GHOST_

#include "platform/platform.hpp"
#include "platform/ghost/ghost.hpp"
#include "server.hpp"

#include <libinput/keyboard/keyboard.hpp>
#include <libinput/mouse/mouse.hpp>
#include <components/cursor.hpp>


int main()
{
	fensterserver::server = new fensterserver::Server();
	fensterserver::server->launch();
	return 0;
}

namespace fensterserver
{
	Server* server = nullptr;

	void platformStartInput()
	{
		inputReceiverInitialize();
	}

	bool platformInitializeKeyboardLayout(std::string layout)
	{
		return g_keyboard::loadLayout(layout);
	}

	void platformLoadCursors()
	{
		auto dir = g_open_directory("/system/graphics/cursor");
		g_fs_directory_entry* entry;
		while((entry = g_read_directory(dir)) != nullptr)
		{
			std::string path = std::string("/system/graphics/cursor") + "/" + entry->name;
			Cursor::load(path);
		}
		Cursor::set("default");
	}

	char platformCharForKey(fenster::KeyInfo info)
	{
		g_key_info ghostKey;
		ghostKey.pressed = info.pressed;
		ghostKey.ctrl = info.ctrl;
		ghostKey.shift = info.shift;
		ghostKey.alt = info.alt;
		ghostKey.scancode = info.scancode;
		ghostKey.key = info.key;
		return g_keyboard::charForKey(ghostKey);
	}
}

#endif

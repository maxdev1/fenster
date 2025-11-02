// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _GHOST_

#include "platform/platform.hpp"
#include "platform/ghost/ghost.hpp"
#include "windowserver.hpp"

#include <libinput/keyboard/keyboard.hpp>
#include <libinput/mouse/mouse.hpp>
#include <components/cursor.hpp>

windowserver_t* server = nullptr;

int main()
{
	server = new windowserver_t();
	server->launch();
	return 0;
}

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
		cursor_t::load(path);
	}
	cursor_t::set("default");
}

char platformCharForKey(g_key_info info)
{
	return g_keyboard::charForKey(info);
}

#endif

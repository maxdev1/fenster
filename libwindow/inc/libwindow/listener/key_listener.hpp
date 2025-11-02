// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_KEYLISTENER
#define LIBWINDOW_KEYLISTENER

#include "listener.hpp"
#include "libwindow/interface.hpp"
#include "libwindow/input/key_info.hpp"
#include <bits/std_function.h>

struct g_key_event
{
	key_info_basic_t info;
};

typedef std::function<void(g_key_event&)> g_key_listener_func;

class g_key_listener : public g_listener
{
public:
	void process(g_ui_component_event_header* header) override
	{
		auto event = (g_ui_component_key_event*) header;

		g_key_event e;
		e.info = event->key_info;
		handleKeyEvent(e);
	}

	virtual void handleKeyEvent(g_key_event& e) = 0;
};

class g_key_listener_dispatcher : public g_key_listener
{
	g_key_listener_func func;

public:
	explicit g_key_listener_dispatcher(g_key_listener_func func):
		func(std::move(func))
	{
	}

	void handleKeyEvent(g_key_event& e) override
	{
		func(e);
	}
};

#endif

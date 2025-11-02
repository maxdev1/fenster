// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_FOCUSLISTENER
#define LIBWINDOW_FOCUSLISTENER

#include "listener.hpp"
#include "../interface.hpp"
#include <bits/std_function.h>

class g_component;

typedef std::function<void(bool)> g_focus_listener_func;

class g_focus_listener : public g_listener
{
public:
	void process(g_ui_component_event_header* header) override
	{
		auto event = (g_ui_component_focus_event*) header;
		handleFocusChanged(event->now_focused);
	}

	virtual void handleFocusChanged(bool nowFocused) = 0;
};

class g_focus_listener_dispatcher : public g_focus_listener
{
	g_focus_listener_func func;

public:
	explicit g_focus_listener_dispatcher(g_focus_listener_func func):
		func(std::move(func))
	{
	}

	void handleFocusChanged(bool nowFocused) override
	{
		func(nowFocused);
	}
};

#endif

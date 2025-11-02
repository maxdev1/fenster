// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <functional>
#include <utility>

#include "libwindow/properties.hpp"
#include "libwindow/window.hpp"

class g_window_close_dispatcher : public g_listener
{
	std::function<void()> func;

public:
	explicit g_window_close_dispatcher(std::function<void()> func) :
		func(std::move(func))
	{
	}

	void process(g_ui_component_event_header* header) override
	{
		func();
	}
};

g_window* g_window::create()
{
	return createComponent<g_window, G_UI_COMPONENT_TYPE_WINDOW>();
}

g_window* g_window::attach(g_ui_component_id id)
{
	return attachComponent<g_window>(id);
}

bool g_window::isResizable()
{
	uint32_t value;
	getNumericProperty(G_UI_PROPERTY_RESIZABLE, &value);
	return value;
}

void g_window::setResizable(bool resizable)
{
	setNumericProperty(G_UI_PROPERTY_RESIZABLE, resizable);
}

bool g_window::onClose(std::function<void()> func)
{
	return addListener(G_UI_COMPONENT_EVENT_TYPE_CLOSE, new g_window_close_dispatcher(std::move(func)));
}

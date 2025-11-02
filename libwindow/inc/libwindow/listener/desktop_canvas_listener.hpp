// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_DESKTOPCANVASLISTENER
#define LIBWINDOW_DESKTOPCANVASLISTENER

#include "listener.hpp"
#include <bits/std_function.h>
#include <utility>

typedef std::function<void(g_ui_windows_event*)> g_desktop_canvas_listener_func;

class g_desktop_canvas_listener : public g_listener
{
public:
    void process(g_ui_component_event_header* header) override
    {
        handleEvent((g_ui_windows_event*)header);
    }

    virtual void handleEvent(g_ui_windows_event* event) = 0;
};

class g_desktop_canvas_listener_dispatcher : public g_desktop_canvas_listener
{
    g_desktop_canvas_listener_func func;

public:
    explicit g_desktop_canvas_listener_dispatcher(g_desktop_canvas_listener_func func): func(std::move(func))
    {
    }

    void handleEvent(g_ui_windows_event* event) override
    {
        func(event);
    }
};

#endif

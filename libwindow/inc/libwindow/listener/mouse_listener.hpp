// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_MOUSELISTENER
#define LIBWINDOW_MOUSELISTENER

#include "libwindow/listener/listener.hpp"
#include <functional>
#include <utility>
#include <bits/std_function.h>

typedef std::function<void(g_ui_component_mouse_event*)> g_mouse_listener_func;

class g_mouse_listener : public g_listener
{
public:
    ~g_mouse_listener() override = default;

    void process(g_ui_component_event_header* header) override
    {
        handleMouseEvent((g_ui_component_mouse_event*)header);
    }

    virtual void handleMouseEvent(g_ui_component_mouse_event* e) = 0;
};

class g_mouse_listener_dispatcher : public g_mouse_listener
{
    g_mouse_listener_func func;

public:
    explicit g_mouse_listener_dispatcher(g_mouse_listener_func func) : func(std::move(func))
    {
    };

    void handleMouseEvent(g_ui_component_mouse_event* e) override
    {
        func(e);
    }
};

#endif

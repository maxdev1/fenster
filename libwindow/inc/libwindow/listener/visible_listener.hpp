// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_VISIBLELISTENER
#define LIBWINDOW_VISIBLELISTENER

#include "listener.hpp"
#include <utility>
#include <bits/std_function.h>

class g_component;

typedef std::function<void(bool)> g_visible_listener_func;

class g_visible_listener : public g_listener
{
public:
    void process(g_ui_component_event_header* header) override
    {
        auto e = (g_ui_component_visible_event*)header;
        handleVisibleChanged(e->visible);
    }

    virtual void handleVisibleChanged(bool visible) = 0;
};

class g_visible_listener_dispatcher : public g_visible_listener
{
    g_visible_listener_func func;

public:
    explicit g_visible_listener_dispatcher(g_visible_listener_func func): func(std::move(func))
    {
    }

    void handleVisibleChanged(bool visible) override
    {
        func(visible);
    }
};


#endif

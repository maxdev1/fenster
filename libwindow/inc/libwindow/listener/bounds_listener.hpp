// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_BOUNDSLISTENER
#define LIBWINDOW_BOUNDSLISTENER

#include "listener.hpp"
#include <utility>
#include <bits/std_function.h>

class g_component;

typedef std::function<void(g_rectangle)> g_bounds_listener_func;

class g_bounds_listener : public g_listener
{
public:
    void process(g_ui_component_event_header* header) override
    {
        auto boundsEvent = (g_ui_component_bounds_event*)header;
        handleBoundsChanged(boundsEvent->bounds);
    }

    virtual void handleBoundsChanged(g_rectangle bounds) = 0;
};

class g_bounds_listener_dispatcher : public g_bounds_listener
{
    g_bounds_listener_func func;

public:
    explicit g_bounds_listener_dispatcher(g_bounds_listener_func func): func(std::move(func))
    {
    }

    void handleBoundsChanged(g_rectangle bounds) override
    {
        func(bounds);
    }
};


#endif

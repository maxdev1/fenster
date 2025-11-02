// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_CANVASBUFFERLISTENER
#define LIBWINDOW_CANVASBUFFERLISTENER

#include <bits/std_function.h>
#include <utility>

typedef std::function<void()> g_canvas_buffer_listener_func;

class g_canvas_buffer_listener
{
public:
    virtual ~g_canvas_buffer_listener() = default;

    virtual void handleBufferChanged() = 0;
};

class g_canvas_buffer_listener_dispatcher : public g_canvas_buffer_listener
{
    g_canvas_buffer_listener_func func;

public:
    explicit g_canvas_buffer_listener_dispatcher(g_canvas_buffer_listener_func func): func(std::move(func))
    {
    }

    void handleBufferChanged() override
    {
        func();
    }
};

#endif

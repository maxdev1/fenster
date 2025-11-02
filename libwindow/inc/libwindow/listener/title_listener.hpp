// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_TITLELISTENER
#define LIBWINDOW_TITLELISTENER

#include "libwindow/listener/listener.hpp"
#include <functional>
#include <utility>
#include <bits/std_function.h>

typedef std::function<void(std::string)> g_title_listener_func;

class g_title_listener : public g_listener
{
public:
    ~g_title_listener() override = default;

    void process(g_ui_component_event_header* header) override
    {
        handleTitleEvent((g_ui_component_title_event*)header);
    }

    virtual void handleTitleEvent(g_ui_component_title_event* e) = 0;
};

class g_title_listener_dispatcher : public g_title_listener
{
    g_title_listener_func func;

public:
    explicit g_title_listener_dispatcher(g_title_listener_func func) : func(std::move(func))
    {
    };

    void handleTitleEvent(g_ui_component_title_event* e) override
    {
        func(std::string(e->title));
    }
};

#endif

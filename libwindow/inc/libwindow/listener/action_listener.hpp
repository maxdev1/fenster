// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_ACTIONLISTENER
#define LIBWINDOW_ACTIONLISTENER

#include "listener.hpp"
#include <utility>
#include <bits/std_function.h>

class g_component;

typedef std::function<void()> g_action_listener_func;

class g_action_listener : public g_listener
{
public:
    void process(g_ui_component_event_header* header) override
    {
        handleAction();
    }

    virtual void handleAction() = 0;
};

class g_action_listener_dispatcher : public g_action_listener
{
    g_action_listener_func func;

public:
    explicit g_action_listener_dispatcher(g_action_listener_func func): func(std::move(func))
    {
    }

    void handleAction() override
    {
        func();
    }
};

#endif

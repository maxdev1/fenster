// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_ACTIONCOMPONENT
#define LIBWINDOW_ACTIONCOMPONENT

#include "listener/action_listener.hpp"
#include "interface.hpp"
#include "component.hpp"

/**
 * Component that is capable of receiving action events
 */
class g_action_component : virtual public g_component
{
protected:
    explicit g_action_component(g_ui_component_id id) : g_component(id)
    {
    }

public:
    ~g_action_component() override = default;

    bool setActionListener(g_action_listener* l);
    bool setActionListener(g_action_listener_func func);
};

#endif

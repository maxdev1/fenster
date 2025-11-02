// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_ACTIONCOMPONENT__
#define __WINDOWSERVER_COMPONENTS_ACTIONCOMPONENT__
#include "component.hpp"

class action_component_t;

typedef std::function<void()> g_internal_action_handler_func;

/**
 * An action component is capable of being observed by an action listener.
 * The component may fire actions which are dispatched to the registered
 * listener for processing.
 */
class action_component_t : virtual public component_t
{
    g_internal_action_handler_func internalHandler;

public:
    explicit action_component_t() : internalHandler(nullptr)
    {
    }

    ~action_component_t() override = default;

    virtual void fireAction();

    void setInternalActionHandler(g_internal_action_handler_func handler)
    {
        this->internalHandler = std::move(handler);
    }
};

#endif

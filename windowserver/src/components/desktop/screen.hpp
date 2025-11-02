// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_SCREEN__
#define __WINDOWSERVER_COMPONENTS_SCREEN__

#include "components/component.hpp"

#include <libwindow/metrics/rectangle.hpp>

class screen_t: virtual public component_t
{
    g_rectangle invalid;

    bool pressing = false;
    g_point pressPoint;

public:
    ~screen_t() override = default;

    void addChild(component_t* comp,
                  component_child_reference_type_t type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;
    void removeChild(component_t* comp) override;

    void sendWindowEvent(g_ui_component_id observerId, window_t* window, SYS_TID_T observerThread, bool present);

    /**
     * Overrides the default invalidation method. On the component, this method
     * just dispatches to the parent, but here we must remember the invalidation.
     */
    virtual void markDirty(g_rectangle rect);

    g_rectangle grabInvalid();
};

#endif

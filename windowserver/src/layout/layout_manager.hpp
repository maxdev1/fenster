// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LAYOUT_MANAGER__
#define __LAYOUT_MANAGER__

#include <libwindow/metrics/insets.hpp>

class component_t;

class layout_manager_t
{
protected:
    component_t* component;

public:
    layout_manager_t() : component(nullptr)
    {
    }

    virtual ~layout_manager_t() = default;

    void setComponent(component_t* newComp)
    {
        component = newComp;
    }

    virtual void layout() = 0;

    virtual void setPadding(g_insets insets)
    {
    }
};

#endif

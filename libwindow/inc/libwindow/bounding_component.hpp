// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_BOUNDING_COMPONENT__
#define __LIBWINDOW_BOUNDING_COMPONENT__

#include "listener/bounds_listener.hpp"

/**
 * Component that is capable of receiving bounds events
 */
class g_bounding_component
{
protected:
    g_component* self;

public:
    explicit g_bounding_component(g_component* self) : self(self)
    {
    }

    virtual ~g_bounding_component() = default;

    bool setBoundsListener(g_bounds_listener* l);
    bool setBoundsListener(g_bounds_listener_func func);
};

#endif

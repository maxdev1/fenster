// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_BOUNDINGCOMPONENT__
#define __WINDOWSERVER_COMPONENTS_BOUNDINGCOMPONENT__

#include <libwindow/metrics/rectangle.hpp>

class component_t;

class bounding_component_t
{
    component_t* self;

public:
    explicit bounding_component_t(component_t* self) : self(self)
    {
    }

    virtual ~bounding_component_t() = default;

    virtual void setBounds(const g_rectangle& bounds);
    virtual void setBoundsInternal(const g_rectangle& bounds) = 0;
};

#endif

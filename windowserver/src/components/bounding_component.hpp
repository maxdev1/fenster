// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_BOUNDINGCOMPONENT
#define FENSTER_SERVER_COMPONENTS_BOUNDINGCOMPONENT

#include <libwindow/metrics/rectangle.hpp>

namespace fensterserver
{
    class Component;

    class BoundingComponent
    {
        Component* self;

    public:
        explicit BoundingComponent(Component* self) : self(self)
        {
        }

        virtual ~BoundingComponent() = default;

        virtual void setBounds(const fenster::Rectangle& bounds);
        virtual void setBoundsInternal(const fenster::Rectangle& bounds) = 0;
    };
}

#endif

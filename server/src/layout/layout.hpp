// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_LAYOUT
#define FENSTER_SERVER_LAYOUT_LAYOUT

#include <libfenster/metrics/insets.hpp>

namespace fensterserver
{
    class Component;

    class Layout
    {
    protected:
        Component* component;

    public:
        Layout() : component(nullptr)
        {
        }

        virtual ~Layout() = default;

        void setComponent(Component* newComp)
        {
            component = newComp;
        }

        virtual void layout() = 0;
    };
}

#endif

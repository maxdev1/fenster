// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_LAYOUTMANAGER
#define FENSTER_SERVER_LAYOUT_LAYOUTMANAGER

#include <libfenster/metrics/insets.hpp>

namespace fensterserver
{
    class Component;

    class LayoutManager
    {
    protected:
        Component* component;

    public:
        LayoutManager() : component(nullptr)
        {
        }

        virtual ~LayoutManager() = default;

        void setComponent(Component* newComp)
        {
            component = newComp;
        }

        virtual void layout() = 0;

        virtual void setPadding(fenster::Insets insets)
        {
        }
    };
}

#endif

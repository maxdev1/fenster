// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_DESKTOP_SCREEN
#define FENSTER_SERVER_COMPONENTS_DESKTOP_SCREEN

#include "components/component.hpp"

#include <libwindow/metrics/rectangle.hpp>

namespace fensterserver
{
    class Screen : virtual public Component
    {
        fenster::Rectangle invalid;

        bool pressing = false;
        fenster::Point pressPoint;

    public:
        ~Screen() override = default;

        void addChild(Component* comp,
                      ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;
        void removeChild(Component* comp) override;

        void sendWindowEvent(fenster::ComponentId observerId, Window* window, SYS_TID_T observerThread, bool present);

        /**
         * Overrides the default invalidation method. On the component, this method
         * just dispatches to the parent, but here we must remember the invalidation.
         */
        virtual void markDirty(fenster::Rectangle rect);

        fenster::Rectangle grabInvalid();
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTREGISTRY
#define FENSTER_SERVER_COMPONENTREGISTRY

#include <libwindow/interface.hpp>

#include "components/component.hpp"

namespace fensterserver
{
    class ComponentRegistry
    {
    public:
        static fenster::ComponentId add(Component* component);
        static void bind(SYS_TID_T process, Component* component);
        static Component* get(fenster::ComponentId id);

        static void removeComponent(SYS_TID_T pid, fenster::ComponentId id);
        static void cleanupProcess(SYS_TID_T pid);

    private:
        static void removeProcessComponents(SYS_TID_T process, Component* component,
                                            std::list<Component*>& removedComponents);
    };
}

#endif

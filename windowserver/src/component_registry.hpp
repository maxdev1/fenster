// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_INTERFACE_COMPONENTREGISTRY__
#define __WINDOWSERVER_INTERFACE_COMPONENTREGISTRY__

#include <libwindow/interface.hpp>

#include "components/component.hpp"

class component_registry_t
{
public:
    static g_ui_component_id add(component_t* component);
    static void bind(SYS_TID_T process, component_t* component);
    static component_t* get(g_ui_component_id id);

    static void removeComponent(SYS_TID_T pid, g_ui_component_id id);
    static void cleanupProcess(SYS_TID_T pid);

private:
    static void removeProcessComponents(SYS_TID_T process, component_t* component,
                                          std::list<component_t*>& removedComponents);
};

#endif

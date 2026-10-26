// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTREGISTRY
#define FENSTER_SERVER_COMPONENTREGISTRY

#include <libfenster/interface.hpp>
#include <functional>

#include "components/component.hpp"

namespace fensterserver
{
    class ComponentRegistry
    {
    public:
        static fenster::ComponentId add(Component* component);
        static void bind(SYS_TID_T process, Component* component);
        static fenster::ComponentIdRange reserveIds(SYS_TID_T process);
        // Hold Access through construction and binding, as for other registry mutations.
        static Component* createReserved(SYS_TID_T process, fenster::ComponentId id,
                                         const std::function<Component*()>& create);
        static Component* get(fenster::ComponentId id);
        static bool owns(SYS_TID_T process, fenster::ComponentId id);

        // Hold across complete operations that borrow component pointers.
        class Access
        {
            bool locked = true;
        public:
            Access();
            ~Access();
            Access(const Access&) = delete;
            Access& operator=(const Access&) = delete;
            void release();
        };

        // Cleanup-generated notifications are copied and sent after readers resume.
        static void sendEvent(SYS_TID_T receiver, void* bytes, size_t size, SYS_TX_T transaction);
        static void forget(fenster::ComponentId id);
        static void destroy(Component* component);
        static void cleanupProcess(SYS_TID_T pid);
    };
}

#endif

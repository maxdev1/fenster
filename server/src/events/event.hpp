// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_EVENTS_EVENT
#define FENSTER_SERVER_EVENTS_EVENT

namespace fensterserver
{
    class Component;

    class Event
    {
    public:
        virtual ~Event()
        {
        }

        /**
         * Lets the event apply on the component.
         */
        virtual Component* visit(Component* component) = 0;
    };
}

#endif

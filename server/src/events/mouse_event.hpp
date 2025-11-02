// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_EVENTS_MOUSEEVENT
#define FENSTER_SERVER_EVENTS_MOUSEEVENT

#include "events/event.hpp"
#include "events/locatable.hpp"
#include <libfenster/interface.hpp>

namespace fensterserver
{
    class MouseEvent : public Event, public Locatable
    {
    public:
        MouseEvent() : type(FENSTER_MOUSE_EVENT_NONE), buttons(FENSTER_MOUSE_BUTTON_NONE), clickCount(1)
        {
        }

        virtual ~MouseEvent()
        {
        }

        fenster::MouseEventType type;
        fenster::MouseButton buttons;
        int clickCount;
        int scroll;

        virtual Component* visit(Component* component);
    };
}

#endif

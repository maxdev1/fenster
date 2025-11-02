// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_EVENTS_KEYEVENT
#define FENSTER_SERVER_EVENTS_KEYEVENT

#include "events/event.hpp"
#include <libwindow/input/key_info.hpp>

namespace fensterserver
{
    class KeyEvent : public Event
    {
    public:
        fenster::KeyInfo info;

        virtual ~KeyEvent()
        {
        }

        virtual Component* visit(Component* component);
    };
}

#endif

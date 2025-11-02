// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_EVENTS_MOUSEEVENT__
#define __WINDOWSERVER_EVENTS_MOUSEEVENT__

#include "events/event.hpp"
#include "events/locatable.hpp"
#include <libwindow/interface.hpp>

class mouse_event_t : public event_t, public locatable_t
{
  public:
    mouse_event_t() : type(G_MOUSE_EVENT_NONE), buttons(G_MOUSE_BUTTON_NONE), clickCount(1)
    {
    }

    virtual ~mouse_event_t() {}

    g_mouse_event_type type;
    g_mouse_button buttons;
    int clickCount;
    int scroll;

    virtual component_t* visit(component_t* component);
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_EVENTS_EVENT__
#define __WINDOWSERVER_EVENTS_EVENT__

class component_t;

class event_t
{
  public:
    virtual ~event_t() {}

    /**
     * Lets the event apply on the component.
     */
    virtual component_t* visit(component_t* component) = 0;
};

#endif

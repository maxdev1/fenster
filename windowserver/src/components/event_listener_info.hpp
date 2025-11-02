// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_EVENTLISTENERINFO
#define FENSTER_SERVER_COMPONENTS_EVENTLISTENERINFO

#include <libwindow/interface.hpp>
#include <list>
#include <string>

namespace fensterserver
{
    typedef struct
    {
        SYS_TID_T target_thread;
        fenster::ComponentId component_id;
    } EventListenerInfo;
}

#endif

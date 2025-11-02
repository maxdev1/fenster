// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_EVENTLISTENERINFO__
#define __WINDOWSERVER_COMPONENTS_EVENTLISTENERINFO__

#include <libwindow/interface.hpp>
#include <list>
#include <string>

typedef struct
{
    SYS_TID_T target_thread;
    g_ui_component_id component_id;
} event_listener_info_t;

#endif

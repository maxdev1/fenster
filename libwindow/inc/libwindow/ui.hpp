// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_UI
#define LIBWINDOW_UI

class g_listener;
class g_canvas;

#include "interface.hpp"
#include "metrics/dimension.hpp"

typedef int g_ui_open_status;
const g_ui_open_status G_UI_OPEN_STATUS_SUCCESSFUL = 0;
const g_ui_open_status G_UI_OPEN_STATUS_COMMUNICATION_FAILED = 1;
const g_ui_open_status G_UI_OPEN_STATUS_FAILED = 2;
const g_ui_open_status G_UI_OPEN_STATUS_EXISTING = 3;

struct g_ui_event_dispatch_data
{
    g_listener* listener;
    uint8_t* data;
    uint32_t length;
};

/**
 * ID of the thread that the window server creates when
 * initializing the UI communication.
 */
extern SYS_TID_T g_ui_delegate_tid;

/**
 * ID of the event dispatcher thread that is continuously waiting
 * for events from the window manager to fire the respective listener
 * that was attached.
 */
extern SYS_TID_T g_ui_event_dispatcher_tid;

class g_ui
{
    static void eventDispatchThread();
    static void eventDispatchQueueAdd(const g_ui_event_dispatch_data& data);

public:
    static g_ui_open_status open();
    static bool registerDesktopCanvas(g_canvas* c);
    static bool getScreenDimension(g_dimension& out);

    static bool addListener(g_ui_component_id id, g_ui_component_event_type eventType);
};

#endif

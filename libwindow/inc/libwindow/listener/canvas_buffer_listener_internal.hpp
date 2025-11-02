// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_CANVASBUFFERLISTENERINTERNAL
#define LIBWINDOW_CANVASBUFFERLISTENERINTERNAL

#include "listener.hpp"

class g_component;
class g_canvas;

/**
 * Listener that a canvas registers on itself to react when a new buffer is ready.
 */
class g_canvas_buffer_listener_internal : public g_listener
{
public:
    g_canvas* canvas;

    explicit g_canvas_buffer_listener_internal(g_canvas* canvas) : canvas(canvas)
    {
    }

    void process(g_ui_component_event_header* header) override;
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/listener/canvas_buffer_listener_internal.hpp"
#include "libwindow/canvas.hpp"

void g_canvas_buffer_listener_internal::process(g_ui_component_event_header* header)
{
	if(header->type == G_UI_COMPONENT_EVENT_TYPE_CANVAS_NEW_BUFFER)
	{
		auto event = (g_ui_component_canvas_wfa_event*) header;
		canvas->acknowledgeNewBuffer(event->newBufferAddress, event->width, event->height);
	}
}

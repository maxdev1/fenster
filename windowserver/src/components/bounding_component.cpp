// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/bounding_component.hpp"
#include "components/component.hpp"

void bounding_component_t::setBounds(const g_rectangle& bounds)
{
	setBoundsInternal(bounds);

	self->callForListeners(G_UI_COMPONENT_EVENT_TYPE_BOUNDS, [bounds](event_listener_info_t& info)
	{
		g_ui_component_bounds_event bounds_event;
		bounds_event.header.type = G_UI_COMPONENT_EVENT_TYPE_BOUNDS;
		bounds_event.header.component_id = info.component_id;
		bounds_event.bounds = bounds;
		platformSendMessage(info.target_thread, &bounds_event, sizeof(g_ui_component_bounds_event), SYS_TX_NONE);
	});
}

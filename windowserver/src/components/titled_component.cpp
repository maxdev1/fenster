// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "titled_component.hpp"
#include "event_listener_info.hpp"
#include <cstring>

void titled_component_t::setTitle(std::string title)
{
	setTitleInternal(title);

	this->callForListeners(G_UI_COMPONENT_EVENT_TYPE_TITLE, [title](event_listener_info_t& info)
	{
		auto event = new g_ui_component_title_event;
		event->header.type = G_UI_COMPONENT_EVENT_TYPE_TITLE;
		event->header.component_id = info.component_id;
		strncpy(event->title, title.c_str(), G_UI_COMPONENT_TITLE_MAXIMUM);
		platformSendMessage(info.target_thread, event, sizeof(g_ui_component_title_event), SYS_TX_NONE);
		delete event;
	});
}

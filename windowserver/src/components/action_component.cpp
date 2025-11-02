// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/action_component.hpp"
#include "components/component.hpp"

void action_component_t::fireAction()
{
	if(internalHandler)
	{
		internalHandler();
		return;
	}

	// otherwise send message to registered thread
	this->callForListeners(G_UI_COMPONENT_EVENT_TYPE_ACTION, [](event_listener_info_t& info)
	{
		g_ui_component_action_event actionEvent;
		actionEvent.header.type = G_UI_COMPONENT_EVENT_TYPE_ACTION;
		actionEvent.header.component_id = info.component_id;
		platformSendMessage(info.target_thread, &actionEvent, sizeof(g_ui_component_action_event), SYS_TX_NONE);
	});
}

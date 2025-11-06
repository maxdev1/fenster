// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "titled_component.hpp"
#include "event_listener_info.hpp"
#include <cstring>

namespace fensterserver
{
	void TitledComponent::setTitle(std::string title)
	{
		setTitleInternal(title);

		this->callForListeners(fenster::ComponentEventType::Title, [title](EventListenerInfo& info)
		{
			auto event = new fenster::ComponentTitleEvent;
			event->header.type = fenster::ComponentEventType::Title;
			event->header.component_id = info.component_id;
			strncpy(event->title, title.c_str(), G_UI_COMPONENT_TITLE_MAXIMUM);
			platformSendMessage(info.target_thread, event, sizeof(fenster::ComponentTitleEvent), SYS_TX_NONE);
			delete event;
		});
	}
}
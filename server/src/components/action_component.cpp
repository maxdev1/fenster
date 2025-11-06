// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/action_component.hpp"
#include "components/component.hpp"

namespace fensterserver
{
	void ActionComponent::fireAction()
	{
		if(internalHandler)
		{
			internalHandler();
			return;
		}

		// otherwise send message to registered thread
		this->callForListeners(fenster::ComponentEventType::Action, [](EventListenerInfo& info)
		{
			fenster::ComponentActionEvent actionEvent;
			actionEvent.header.type = fenster::ComponentEventType::Action;
			actionEvent.header.component_id = info.component_id;
			platformSendMessage(info.target_thread, &actionEvent, sizeof(fenster::ComponentActionEvent), SYS_TX_NONE);
		});
	}
}


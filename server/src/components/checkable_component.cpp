// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/checkable_component.hpp"
#include "components/component.hpp"

namespace fensterserver
{
	void CheckableComponent::setChecked(bool checked, bool triggeredByGroup)
	{
		setCheckedInternal(checked);

		if(internalHandler)
		{
			internalHandler(checked, triggeredByGroup);
			return;
		}

		this->callForListeners(FENSTER_COMPONENT_EVENT_TYPE_CHECKED, [checked](EventListenerInfo& info)
		{
			fenster::ComponentCheckedEvent event;
			event.header.type = FENSTER_COMPONENT_EVENT_TYPE_CHECKED;
			event.header.component_id = info.component_id;
			event.checked = checked;
			platformSendMessage(info.target_thread, &event, sizeof(fenster::ComponentCheckedEvent), SYS_TX_NONE);
		});
	}
}

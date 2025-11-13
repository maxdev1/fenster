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

		for (const auto& internalHandler : internalHandlers) {
			internalHandler(checked, triggeredByGroup);
		}

		this->callForListeners(fenster::ComponentEventType::Checked, [checked](EventListenerInfo& info)
		{
			fenster::ComponentCheckedEvent event;
			event.header.type = fenster::ComponentEventType::Checked;
			event.header.component_id = info.component_id;
			event.checked = checked;
			platformSendMessage(info.target_thread, &event, sizeof(fenster::ComponentCheckedEvent), SYS_TX_NONE);
		});
	}

}

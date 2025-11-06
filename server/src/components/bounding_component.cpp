// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/bounding_component.hpp"
#include "components/component.hpp"

namespace fensterserver
{
	void BoundingComponent::setBounds(const fenster::Rectangle& bounds)
	{
		setBoundsInternal(bounds);

		self->callForListeners(fenster::ComponentEventType::Bounds, [bounds](EventListenerInfo& info)
		{
			fenster::ComponentBoundsEvent bounds_event;
			bounds_event.header.type = fenster::ComponentEventType::Bounds;
			bounds_event.header.component_id = info.component_id;
			bounds_event.bounds = bounds;
			platformSendMessage(info.target_thread, &bounds_event, sizeof(fenster::ComponentBoundsEvent), SYS_TX_NONE);
		});
	}
}

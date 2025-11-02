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

		self->callForListeners(FENSTER_COMPONENT_EVENT_TYPE_BOUNDS, [bounds](EventListenerInfo& info)
		{
			fenster::ComponentBoundsEvent bounds_event;
			bounds_event.header.type = FENSTER_COMPONENT_EVENT_TYPE_BOUNDS;
			bounds_event.header.component_id = info.component_id;
			bounds_event.bounds = bounds;
			platformSendMessage(info.target_thread, &bounds_event, sizeof(fenster::ComponentBoundsEvent), SYS_TX_NONE);
		});
	}
}

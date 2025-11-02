// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/listener/canvas_buffer_listener_internal.hpp"
#include "libfenster/canvas.hpp"

namespace fenster
{
	void CanvasBufferListenerInternal::process(ComponentEventHeader* header)
	{
		if(header->type == FENSTER_COMPONENT_EVENT_TYPE_CANVAS_NEW_BUFFER)
		{
			auto event = (ComponentCanvasWaitForAcknowledgeEvent*) header;
			canvas->acknowledgeNewBuffer(event->newBufferAddress, event->width, event->height);
		}
	}
}

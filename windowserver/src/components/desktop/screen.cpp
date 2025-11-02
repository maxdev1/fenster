// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/desktop/screen.hpp"
#include "components/window.hpp"

#include <cstring>

void screen_t::addChild(component_t* comp, component_child_reference_type_t type)
{
	component_t::addChild(comp, type);

	if(comp->isWindow())
	{
		this->callForListeners(G_UI_COMPONENT_EVENT_TYPE_WINDOWS, [this, comp](event_listener_info_t& info)
		{
			auto window = dynamic_cast<window_t*>(comp);
			sendWindowEvent(info.component_id, window, info.target_thread, true);
		});
	}
}

void screen_t::removeChild(component_t* comp)
{
	if(comp->isWindow())
	{
		this->callForListeners(G_UI_COMPONENT_EVENT_TYPE_WINDOWS, [this, comp](event_listener_info_t& info)
		{
			sendWindowEvent(info.component_id, dynamic_cast<window_t*>(comp), info.target_thread,
			                false);
		});
	}

	component_t::removeChild(comp);
}

void screen_t::sendWindowEvent(g_ui_component_id observerId, window_t* window, SYS_TID_T observerThread, bool present)
{
	g_ui_windows_event windowEvent;
	windowEvent.header.type = G_UI_COMPONENT_EVENT_TYPE_WINDOWS;
	windowEvent.header.component_id = observerId;
	windowEvent.window_id = window->id;
	windowEvent.present = present;
	platformSendMessage(observerThread, &windowEvent, sizeof(g_ui_windows_event), SYS_TX_NONE);
}

void screen_t::markDirty(g_rectangle rect)
{
	platformAcquireMutex(lock);

	if(invalid.width == 0 && invalid.height == 0)
	{
		invalid = rect;
	}
	else
	{
		invalid.extend(rect.getStart());
		invalid.extend(rect.getEnd());
	}

	// Fix invalid area
	if(invalid.x < 0)
	{
		invalid.width += invalid.x;
		invalid.x = 0;
	}
	if(invalid.y < 0)
	{
		invalid.height += invalid.y;
		invalid.y = 0;
	}
	if(invalid.x + invalid.width > getBounds().width)
	{
		invalid.width = getBounds().width - invalid.x;
	}
	if(invalid.y + invalid.height > getBounds().height)
	{
		invalid.height = getBounds().height - invalid.y;
	}
	platformReleaseMutex(lock);
}

g_rectangle screen_t::grabInvalid()
{
	platformAcquireMutex(lock);
	g_rectangle ret = invalid;
	invalid = g_rectangle();
	platformReleaseMutex(lock);
	return ret;
}

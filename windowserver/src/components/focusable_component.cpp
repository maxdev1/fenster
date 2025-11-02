// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/focusable_component.hpp"
#include "components/component.hpp"

#include <libwindow/properties.hpp>

component_t* focusable_component_t::setFocused(bool focused)
{
	if(isFocusable())
	{
		setFocusedInternal(focused);

		self->callForListeners(G_UI_COMPONENT_EVENT_TYPE_FOCUS, [focused](event_listener_info_t& info)
		{
			g_ui_component_focus_event event;
			event.header.type = G_UI_COMPONENT_EVENT_TYPE_FOCUS;
			event.header.component_id = info.component_id;
			event.now_focused = focused;
			platformSendMessage(info.target_thread, &event, sizeof(g_ui_component_focus_event), SYS_TX_NONE);
		});

		return self;
	}

	if(this->dispatchesFocus)
	{
		auto parent = self->getParent();
		if(parent)
			return parent->setFocused(focused);
	}
	return nullptr;
}


bool focusable_component_t::setNumericProperty(int property, uint32_t value)
{
	if(property == G_UI_PROPERTY_FOCUSABLE)
	{
		this->focusable = value == 1;
		return true;
	}
	if(property == G_UI_PROPERTY_DISPATCHES_FOCUS)
	{
		this->dispatchesFocus = value == 1;
		return true;
	}
	return false;
}

bool focusable_component_t::getNumericProperty(int property, uint32_t* out)
{
	if(property == G_UI_PROPERTY_FOCUSABLE)
	{
		*out = this->focusable ? 1 : 0;
		return true;
	}
	if(property == G_UI_PROPERTY_DISPATCHES_FOCUS)
	{
		*out = this->dispatchesFocus ? 1 : 0;
		return true;
	}
	return false;
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libwindow/focusable_component.hpp"
#include "libwindow/properties.hpp"
#include "libwindow/listener/focus_listener.hpp"

bool g_focusable_component::isFocused()
{
	uint32_t out;
	getNumericProperty(G_UI_PROPERTY_FOCUSED, &out);
	return out == 1;
}

bool g_focusable_component::setFocused(bool focused)
{
	if(!g_ui_initialized)
		return false;

	SYS_TX_T tx = platformCreateMessageTransaction();

	g_ui_component_focus_request request;
	request.header.id = G_UI_PROTOCOL_FOCUS;
	request.id = this->id;
	platformSendMessage(g_ui_delegate_tid, &request, sizeof(g_ui_component_focus_request), tx);
	platformYieldTo(g_ui_delegate_tid);

	size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(g_ui_component_focus_response);
	uint8_t buffer[bufferSize];

	bool success = false;
	if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
	{
		auto response = (g_ui_component_focus_response*) SYS_MESSAGE_CONTENT(buffer);
		success = (response->status == G_UI_PROTOCOL_SUCCESS);
	}

	return success;
}

void g_focusable_component::addFocusListener(std::function<void(bool)> func)
{
	this->addListener(G_UI_COMPONENT_EVENT_TYPE_FOCUS, new g_focus_listener_dispatcher(std::move(func)));
}

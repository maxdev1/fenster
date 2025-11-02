// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/scrollpane.hpp"

g_scrollpane* g_scrollpane::create()
{
	return createComponent<g_scrollpane, G_UI_COMPONENT_TYPE_SCROLLPANE>();
}


bool g_scrollpane::setContent(g_component* content)
{
	if(!g_ui_initialized)
		return false;

	SYS_TX_T tx = platformCreateMessageTransaction();

	g_ui_scrollpane_set_content request;
	request.header.id = G_UI_PROTOCOL_SCROLLPANE_SET_CONTENT;
	request.scrollpane = this->id;
	request.content = content->getId();
	platformSendMessage(g_ui_delegate_tid, &request, sizeof(request), tx);
	platformYieldTo(g_ui_delegate_tid);

	size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(g_ui_simple_response);
	uint8_t buffer[buflen];
	if(platformReceiveMessage(buffer, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
	{
		auto response = (g_ui_simple_response*) SYS_MESSAGE_CONTENT(buffer);
		return response->status == G_UI_PROTOCOL_SUCCESS;
	}
	return false;
}

bool g_scrollpane::setFixed(bool width, bool height)
{
	if(!g_ui_initialized)
		return false;

	SYS_TX_T tx = platformCreateMessageTransaction();

	g_ui_scrollpane_set_fixed request;
	request.header.id = G_UI_PROTOCOL_SCROLLPANE_SET_FIXED;
	request.scrollpane = this->id;
	request.width = width;
	request.height = height;
	platformSendMessage(g_ui_delegate_tid, &request, sizeof(request), tx);
	platformYieldTo(g_ui_delegate_tid);

	size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(g_ui_simple_response);
	uint8_t buffer[buflen];
	if(platformReceiveMessage(buffer, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
	{
		auto response = (g_ui_simple_response*) SYS_MESSAGE_CONTENT(buffer);
		return response->status == G_UI_PROTOCOL_SUCCESS;
	}
	return false;
}

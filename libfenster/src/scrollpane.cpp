// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/scrollpane.hpp"

namespace fenster
{
	ScrollPane* ScrollPane::create()
	{
		return createComponent<ScrollPane, FENSTER_COMPONENT_TYPE_SCROLLPANE>();
	}

	bool ScrollPane::setContent(Component* content)
	{
		if(!ApplicationInitialized)
			return false;

		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandScrollPaneSetContent request;
		request.header.id = FENSTER_PROTOCOL_SCROLLPANE_SET_CONTENT;
		request.scrollpane = this->id;
		request.content = content->getId();
		platformSendMessage(DelegateTaskId, &request, sizeof(request), tx);
		platformYieldTo(DelegateTaskId);

		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse);
		uint8_t buffer[buflen];
		if(platformReceiveMessage(buffer, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandSimpleResponse*) SYS_MESSAGE_CONTENT(buffer);
			return response->status == FENSTER_PROTOCOL_SUCCESS;
		}
		return false;
	}

	bool ScrollPane::setFixed(bool width, bool height)
	{
		if(!ApplicationInitialized)
			return false;

		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandScrollPaneSetFixed request;
		request.header.id = FENSTER_PROTOCOL_SCROLLPANE_SET_FIXED;
		request.scrollpane = this->id;
		request.width = width;
		request.height = height;
		platformSendMessage(DelegateTaskId, &request, sizeof(request), tx);
		platformYieldTo(DelegateTaskId);

		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse);
		uint8_t buffer[buflen];
		if(platformReceiveMessage(buffer, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandSimpleResponse*) SYS_MESSAGE_CONTENT(buffer);
			return response->status == FENSTER_PROTOCOL_SUCCESS;
		}
		return false;
	}
}

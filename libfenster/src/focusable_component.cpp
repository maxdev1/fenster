// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libfenster/focusable_component.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/listener/focus_listener.hpp"

namespace fenster
{
	bool FocusableComponent::isFocused()
	{
		uint32_t out;
		getNumericProperty(ComponentProperty::Focused, &out);
		return out == 1;
	}

	bool FocusableComponent::setFocused(bool focused)
	{
		if(!ApplicationInitialized)
			return false;

		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandFocusRequest request;
		request.header.id = FENSTER_PROTOCOL_FOCUS;
		request.id = this->id;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandFocusRequest), tx);
		platformYieldTo(DelegateTaskId);

		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandFocusResponse);
		uint8_t buffer[bufferSize];

		bool success = false;
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandFocusResponse*) SYS_MESSAGE_CONTENT(buffer);
			success = (response->status == FENSTER_PROTOCOL_SUCCESS);
		}

		return success;
	}

	void FocusableComponent::addFocusListener(std::function<void(bool)> func)
	{
		this->addListener(FENSTER_COMPONENT_EVENT_TYPE_FOCUS, new FocusListenerDispatcher(std::move(func)));
	}
}


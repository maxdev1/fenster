// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"

#include <utility>
#include <cstring>
#include <malloc.h>

namespace fenster
{
	Component::~Component()
	{
		destroy();
	}

	void Component::destroy()
	{
		if(destroyed)
			return;
		destroyed = true;

		CommandDestroyComponentRequest request;
		request.header.id = fenster::ProtocolCommandId::DestroyComponent;
		request.id = this->id;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandDestroyComponentRequest), SYS_TX_NONE);
		platformYieldTo(DelegateTaskId);
	}

	bool Component::addChild(Component* child)
	{
		if(!ApplicationInitialized)
			return false;

		// send initialization request
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandAddChildRequest request;
		request.header.id = fenster::ProtocolCommandId::AddComponent;
		request.parent = this->id;
		request.child = child->id;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandAddChildRequest), tx);

		// read response
		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandAddChildResponse);
		uint8_t buffer[bufferSize];

		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandAddChildResponse*) SYS_MESSAGE_CONTENT(buffer);
			if(response->status == ProtocolStatus::Success)
			{
				return true;
			}
		}

		return false;
	}

	bool Component::setBounds(const Rectangle& rect)
	{
		if(!ApplicationInitialized)
			return false;

		// send initialization request
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandSetBoundsRequest request;
		request.header.id = fenster::ProtocolCommandId::SetBounds;
		request.id = this->id;
		request.bounds = rect;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandSetBoundsRequest), tx);
		platformYieldTo(DelegateTaskId);

		// read response
		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse);
		uint8_t buffer[buflen];
		if(platformReceiveMessage(buffer, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandSimpleResponse*) SYS_MESSAGE_CONTENT(buffer);
			return response->status == ProtocolStatus::Success;
		}
		return false;
	}

	Rectangle Component::getBounds()
	{
		if(!ApplicationInitialized)
			return Rectangle();

		// send initialization request
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandGetBoundsRequest request;
		request.header.id = fenster::ProtocolCommandId::GetBounds;
		request.id = this->id;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandGetBoundsRequest), tx);
		platformYieldTo(DelegateTaskId);

		// read response
		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandGetBoundsResponse);
		uint8_t buffer[bufferSize];

		Rectangle result;
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandGetBoundsResponse*) SYS_MESSAGE_CONTENT(buffer);
			if(response->status == ProtocolStatus::Success)
				result = response->bounds;
		}

		return result;
	}

	bool Component::setNumericProperty(ComponentProperty property, uint32_t value)
	{
		if(!ApplicationInitialized)
			return false;

		// send initialization request
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandSetNumericPropertyRequest request;
		request.header.id = fenster::ProtocolCommandId::SetNumericProperty;
		request.id = this->id;
		request.property = property;
		request.value = value;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandSetNumericPropertyRequest), tx);
		platformYieldTo(DelegateTaskId);

		// read response
		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSetStringPropertyResponse);
		uint8_t buffer[bufferSize];

		bool success = false;
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandSetStringPropertyResponse*)
					SYS_MESSAGE_CONTENT(buffer);
			success = (response->status == ProtocolStatus::Success);
		}

		return success;
	}

	bool Component::getNumericProperty(ComponentProperty property, uint32_t* out)
	{
		if(!ApplicationInitialized)
			return false;

		// send initialization request
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandGetNumericPropertyRequest request;
		request.header.id = fenster::ProtocolCommandId::GetNumericProperty;
		request.id = this->id;
		request.property = property;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandGetNumericPropertyRequest), tx);
		platformYieldTo(DelegateTaskId);

		// read response
		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandGetNumericPropertyResponse);
		uint8_t buffer[bufferSize];

		bool success = false;
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandGetNumericPropertyResponse*)
					SYS_MESSAGE_CONTENT(buffer);

			if(response->status == ProtocolStatus::Success)
			{
				*out = response->value;
				success = true;
			}
		}

		return success;
	}


	bool Component::setStringProperty(ComponentProperty property, std::string value)
	{
		if(!ApplicationInitialized)
			return false;

		SYS_TX_T tx = platformCreateMessageTransaction();

		auto requestSize = sizeof(CommandSetStringPropertyRequest) + value.length() + 1;
		auto request = static_cast<CommandSetStringPropertyRequest*>(
			operator new(sizeof(CommandSetStringPropertyRequest) + value.length() + 1)
		);
		request->header.id = fenster::ProtocolCommandId::SetStringProperty;
		request->id = this->id;
		request->property = property;
		strcpy(request->value, value.c_str());

		platformSendMessage(DelegateTaskId, request, requestSize, tx);
		platformYieldTo(DelegateTaskId);

		size_t responseBufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse);
		uint8_t responseBuffer[responseBufferSize];
		bool success = false;
		if(platformReceiveMessage(responseBuffer, responseBufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandSimpleResponse*) SYS_MESSAGE_CONTENT(responseBuffer);
			success = (response->status == ProtocolStatus::Success);
		}

		delete request;
		return success;
	}

	bool Component::getStringProperty(ComponentProperty property, std::string& out)
	{
		if(!ApplicationInitialized)
			return false;

		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandGetStringPropertyRequest request;
		request.header.id = fenster::ProtocolCommandId::GetStringProperty;
		request.id = this->id;
		request.property = property;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandGetStringPropertyRequest), tx);
		platformYieldTo(DelegateTaskId);

		bool success = false;
		for(int size = 128; size <= 1024; size *= 2)
		{
			size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandGetStringPropertyResponse) + size;
			auto buffer = new uint8_t[bufferSize];
			auto receiveStatus = platformReceiveMessage(buffer, bufferSize, tx);
			if(receiveStatus == SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				auto response = (CommandGetStringPropertyResponse*) SYS_MESSAGE_CONTENT(buffer);
				if(response->status == ProtocolStatus::Success)
				{
					success = true;
					out = std::string(response->value);
				}
			}
			delete buffer;

			if(success || receiveStatus != SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER)
				break;
		}
		return success;
	}

	bool Component::addListener(ComponentEventType eventType, Listener* newListener)
	{
		if(!ApplicationInitialized)
			return false;

		if(newListener == nullptr)
			return false;

		platformAcquireMutex(listenersLock);
		listeners[eventType].push_back(newListener);
		platformReleaseMutex(listenersLock);

		return Application::addListener(this->id, eventType);
	}

	void Component::handle(ComponentEventHeader* header)
	{
		auto eventType = header->type;

		platformAcquireMutex(listenersLock);
		auto it = listeners.find(eventType);
		if(it != listeners.end())
		{
			for(auto& listener: it->second)
			{
				listener->process(header);
			}
		}
		else
		{
			platformLog("incoming event (%i) but no one to handle", eventType);
		}
		platformReleaseMutex(listenersLock);
	}

	bool Component::addMouseListener(MouseListener* listener)
	{
		return addListener(ComponentEventType::Mouse, listener);
	}

	bool Component::addMouseListener(MouseListenerFunc func)
	{
		return addListener(ComponentEventType::Mouse, new MouseListenerDispatcher(std::move(func)));
	}

	bool Component::addKeyListener(KeyListener* listener)
	{
		return addListener(ComponentEventType::Key, listener);
	}

	bool Component::addKeyListener(KeyListenerFunc func)
	{
		return addListener(ComponentEventType::Key, new KeyListenerDispatcher(std::move(func)));
	}

	bool Component::addVisibleListener(VisibleListener* listener)
	{
		return addListener(ComponentEventType::Visible, listener);
	}

	bool Component::addVisibleListener(VisibleListenerFunc func)
	{
		return addListener(ComponentEventType::Visible, new VisibleListenerDispatcher(std::move(func)));
	}

	bool Component::setBackground(ColorArgb argb)
	{
		return setNumericProperty(ComponentProperty::Background, argb);
	}

	bool Component::isVisible()
	{
		uint32_t visible;
		if(getNumericProperty(ComponentProperty::Visible, &visible))
		{
			return visible == 1;
		}
		return false;
	}

	bool Component::setVisible(bool visible)
	{
		return setNumericProperty(ComponentProperty::Visible, visible ? 1 : 0);
	}


	bool Component::isFocusable()
	{
		uint32_t focusable;
		if(getNumericProperty(ComponentProperty::Focusable, &focusable))
		{
			return focusable == 1;
		}
		return false;
	}

	bool Component::setFocusable(bool focusable)
	{
		return setNumericProperty(ComponentProperty::Focusable, focusable ? 1 : 0);
	}


	bool Component::isDispatchesFocus()
	{
		uint32_t d;
		if(getNumericProperty(ComponentProperty::DispatchesFocus, &d))
		{
			return d == 1;
		}
		return false;
	}

	bool Component::setDispatchesFocus(bool d)
	{
		return setNumericProperty(ComponentProperty::DispatchesFocus, d ? 1 : 0);
	}

	bool Component::setPreferredSize(const Dimension& size)
	{
		return setSize(fenster::ProtocolCommandId::SetPreferredSize, size);
	}

	bool Component::setMinimumSize(const Dimension& size)
	{
		return setSize(fenster::ProtocolCommandId::SetMinimumSize, size);
	}

	bool Component::setMaximumSize(const Dimension& size)
	{
		return setSize(fenster::ProtocolCommandId::SetMaximumSize, size);
	}

	bool Component::setSize(ProtocolCommandId command, const Dimension& size)
	{
		if(!ApplicationInitialized)
			return false;

		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandSetSizeRequest request;
		request.header.id = command;
		request.id = this->id;
		request.size = size;
		platformSendMessage(DelegateTaskId, &request, sizeof(request), tx);
		platformYieldTo(DelegateTaskId);

		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse);
		uint8_t buffer[buflen];
		if(platformReceiveMessage(buffer, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandSimpleResponse*) SYS_MESSAGE_CONTENT(buffer);
			return response->status == ProtocolStatus::Success;
		}
		return false;
	}

	bool Component::setLayout(Layout* layout)
	{
		return setNumericProperty(ComponentProperty::Layout, (uint32_t) layout->getType());
	}

}

// fenster — lightweight window server and UI toolkit
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/component_update_batch.hpp"

#include <cstring>
#include <utility>

namespace fenster
{
	ComponentUpdateBatch::ComponentUpdateBatch()
	{
		buffer.reserve(G_UI_MAXIMUM_UPDATE_SIZE);
		buffer.push_back(static_cast<uint8_t>(ProtocolCommandId::UpdateComponents));
	}

	bool ComponentUpdateBatch::append(const void* request, size_t length)
	{
		if(!successful || !ApplicationInitialized ||
		   length > G_UI_MAXIMUM_UPDATE_SIZE - sizeof(CommandUpdateComponentsRequest) - sizeof(ComponentUpdateHeader))
			return successful = false;
		if(buffer.size() + sizeof(ComponentUpdateHeader) + length > G_UI_MAXIMUM_UPDATE_SIZE && !commit())
			return false;

		ComponentUpdateHeader header{static_cast<uint16_t>(length)};
		const auto start = buffer.size();
		buffer.resize(start + sizeof(header) + length);
		memcpy(buffer.data() + start, &header, sizeof(header));
		memcpy(buffer.data() + start + sizeof(header), request, length);
		return true;
	}

	bool ComponentUpdateBatch::commit()
	{
		if(!successful || buffer.size() == sizeof(CommandUpdateComponentsRequest))
			return successful;

		const auto tx = platformCreateMessageTransaction();
		if(!platformSendMessage(DelegateTaskId, buffer.data(), buffer.size(), tx))
			return successful = false;
		platformYieldTo(DelegateTaskId);
		uint8_t response[SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse)];
		if(platformReceiveMessage(response, sizeof(response), tx) != SYS_MESSAGE_RECEIVE_SUCCESS ||
		   reinterpret_cast<CommandSimpleResponse*>(SYS_MESSAGE_CONTENT(response))->status != ProtocolStatus::Success)
			return successful = false;

		buffer.resize(sizeof(CommandUpdateComponentsRequest));
		return true;
	}

	bool ComponentUpdateBatch::setNumericProperty(Component* component, ComponentProperty property, uint32_t value)
	{
		CommandSetNumericPropertyRequest request{{ProtocolCommandId::SetNumericProperty}, component->getId(), property, value};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::setStringProperty(Component* component, ComponentProperty property, const std::string& value)
	{
		if(value.size() > G_UI_MAXIMUM_UPDATE_SIZE - sizeof(CommandUpdateComponentsRequest) -
		                  sizeof(ComponentUpdateHeader) - sizeof(CommandSetStringPropertyRequest) - 1)
			return successful = false;
		std::vector<uint8_t> bytes(sizeof(CommandSetStringPropertyRequest) + value.size() + 1);
		auto request = reinterpret_cast<CommandSetStringPropertyRequest*>(bytes.data());
		request->header.id = ProtocolCommandId::SetStringProperty;
		request->id = component->getId();
		request->property = property;
		memcpy(request->value, value.c_str(), value.size() + 1);
		return append(bytes.data(), bytes.size());
	}

	bool ComponentUpdateBatch::setBounds(Component* component, const Rectangle& bounds)
	{
		CommandSetBoundsRequest request{{ProtocolCommandId::SetBounds}, component->getId(), bounds};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::setMinimumSize(Component* component, const Dimension& size)
	{
		CommandSetSizeRequest request{{ProtocolCommandId::SetMinimumSize}, component->getId(), size};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::addChild(Component* parent, Component* child)
	{
		CommandAddChildRequest request{{ProtocolCommandId::AddComponent}, parent->getId(), child->getId()};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::addMouseListener(Component* component, MouseListenerFunc listener)
	{
		if(!successful || !ApplicationInitialized)
			return successful = false;
		CommandAddListenerRequest request{{ProtocolCommandId::AddListener}, component->getId(),
		                                 ComponentEventType::Mouse, EventDispatcherTaskId};
		if(!append(&request, sizeof(request)))
			return false;
		platformAcquireMutex(component->listenersLock);
		component->listeners[ComponentEventType::Mouse].push_back(new MouseListenerDispatcher(std::move(listener)));
		platformReleaseMutex(component->listenersLock);
		return true;
	}
}

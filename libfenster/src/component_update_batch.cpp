// fenster — lightweight window server and UI toolkit
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/component_update_batch.hpp"

#include <cstring>
#include <utility>
#include <memory>
#include <limits>

namespace fenster
{
	static SYS_MUTEX_T idLock = platformInitializeMutex(false);
	static std::vector<ComponentIdRange> idRanges;

	bool ComponentUpdateBatch::addIdRange(ComponentIdRange range)
	{
		if(range.first <= 0 || !range.count || range.count > G_UI_COMPONENT_ID_RESERVATION_SIZE ||
		   uint64_t(range.first) + range.count - 1 > std::numeric_limits<ComponentId>::max())
			return false;
		platformAcquireMutex(idLock);
		idRanges.push_back(range);
		platformReleaseMutex(idLock);
		return true;
	}

	ComponentId ComponentUpdateBatch::createComponent(ComponentType type)
	{
		if(!successful || !ApplicationInitialized || !isCreatableComponentType(type))
		{
			successful = false;
			return 0;
		}
		for(;;)
		{
			platformAcquireMutex(idLock);
			if(!idRanges.empty())
			{
				auto& range = idRanges.back();
				auto id = range.first;
				if(--range.count) ++range.first;
				else idRanges.pop_back();
				platformReleaseMutex(idLock);
				CommandCreateReservedComponentRequest request{{ProtocolCommandId::CreateReservedComponent}, id, type};
				return append(&request, sizeof(request)) ? id : 0;
			}
			platformReleaseMutex(idLock);
			// No ID-pool lock is held while flushing or waiting for another block.
			MessageHeader request{ProtocolCommandId::ReserveComponentIds};
			auto tx = platformCreateMessageTransaction();
			uint8_t buffer[SYS_MESSAGE_HEADER_SIZE + sizeof(CommandReserveComponentIdsResponse)];
			if(!sendUiMessage(DelegateTaskId, &request, sizeof(request), tx) ||
			   platformReceiveMessage(buffer, sizeof(buffer), tx) != SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				successful = false;
				return 0;
			}
			auto response = reinterpret_cast<CommandReserveComponentIdsResponse*>(SYS_MESSAGE_CONTENT(buffer));
			if(response->status != ProtocolStatus::Success || !addIdRange(response->component_ids))
			{
				successful = false;
				return 0;
			}
		}
	}

	// Ghost places shared-library TLS immediately after executable TLS. Preserve
	// its 16-byte alignment for libraries such as Pixman that use aligned SIMD loads.
	alignas(16) thread_local ComponentUpdateBatch* ComponentUpdateBatch::activeBatch = nullptr;

	ComponentUpdateBatch::Scope::Scope(ComponentUpdateBatch& batch): batch(batch), previous(activeBatch)
	{
		if(previous && !previous->commit())
			batch.successful = false;
		activeBatch = &batch;
	}

	ComponentUpdateBatch::Scope::~Scope()
	{
		if(!batch.commit() && previous)
			previous->successful = false;
		activeBatch = previous;
	}

	bool sendUiMessage(SYS_TID_T tid, void* bytes, size_t length, SYS_TX_T transaction)
	{
		auto batch = ComponentUpdateBatch::current();
		return (!batch || batch->commit()) && platformSendMessage(tid, bytes, length, transaction);
	}

	bool sendUiMutation(void* bytes, size_t length)
	{
		if(auto batch = ComponentUpdateBatch::current()) return batch->append(bytes, length);
		auto tx = platformCreateMessageTransaction();
		uint8_t response[SYS_MESSAGE_HEADER_SIZE + sizeof(CommandSimpleResponse)];
		return ApplicationInitialized && sendUiMessage(DelegateTaskId, bytes, length, tx) &&
			   platformReceiveMessage(response, sizeof(response), tx) == SYS_MESSAGE_RECEIVE_SUCCESS &&
			   reinterpret_cast<CommandSimpleResponse*>(SYS_MESSAGE_CONTENT(response))->status == ProtocolStatus::Success;
	}

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
		if(activeBatch && activeBatch != this && !activeBatch->commit())
			return successful = false;

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
		return setSize(ProtocolCommandId::SetMinimumSize, component, size);
	}

	bool ComponentUpdateBatch::setMaximumSize(Component* component, const Dimension& size)
	{
		return setSize(ProtocolCommandId::SetMaximumSize, component, size);
	}

	bool ComponentUpdateBatch::setPreferredSize(Component* component, const Dimension& size)
	{
		return setSize(ProtocolCommandId::SetPreferredSize, component, size);
	}

	bool ComponentUpdateBatch::setSize(ProtocolCommandId command, Component* component, const Dimension& size)
	{
		CommandSetSizeRequest request{{command}, component->getId(), size};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::addChild(Component* parent, Component* child)
	{
		CommandAddChildRequest request{{ProtocolCommandId::AddComponent}, parent->getId(), child->getId()};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::addMouseListener(Component* component, MouseListenerFunc listener)
	{
		return addListener(component, ComponentEventType::Mouse, new MouseListenerDispatcher(std::move(listener)));
	}

	bool ComponentUpdateBatch::addKeyListener(Component* component, KeyListenerFunc listener)
	{
		return addListener(component, ComponentEventType::Key, new KeyListenerDispatcher(std::move(listener)));
	}

	bool ComponentUpdateBatch::registerListener(ComponentId component, ComponentEventType type)
	{
		if(!successful || !ApplicationInitialized)
			return successful = false;
		CommandAddListenerRequest request{{ProtocolCommandId::AddListener}, component, type, EventDispatcherTaskId};
		return append(&request, sizeof(request));
	}

	bool ComponentUpdateBatch::addListener(Component* component, ComponentEventType type, Listener* listener)
	{
		std::unique_ptr<Listener> owned(listener);
		if(!registerListener(component->getId(), type))
			return false;
		platformAcquireMutex(component->listenersLock);
		component->listeners[type].push_back(owned.release());
		platformReleaseMutex(component->listenersLock);
		return true;
	}
}

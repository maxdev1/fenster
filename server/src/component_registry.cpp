// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>
#include <map>
#include <cstring>
#include <utility>
#include <vector>
#include <bitset>
#include <limits>

#include "component_registry.hpp"
#include "process_registry.hpp"
#include "input/mouse_grab.hpp"

namespace fensterserver
{
	static std::map<fenster::ComponentId, Component*> components;
	static SYS_MUTEX_T componentsLock = fenster::platformInitializeMutex(true);
	static std::map<SYS_TID_T, std::map<fenster::ComponentId, Component*>> components_by_process;
	static fenster::ComponentId next_id = 1;
	struct ReservedIds
	{
		SYS_TID_T owner;
		std::bitset<fenster::G_UI_COMPONENT_ID_RESERVATION_SIZE> used;
	};
	static std::map<fenster::ComponentId, ReservedIds> reservations;
	// Keep Ghost's following shared-library TLS aligned for Pixman.
	alignas(16) static thread_local fenster::ComponentId creationId = 0;
	static SYS_MUTEX_T readersLock = fenster::platformInitializeMutex(false);
	static SYS_MUTEX_T readersDrained = fenster::platformInitializeMutex(false);
	static size_t readers = 0;
	struct PendingEvent
	{
		SYS_TID_T receiver;
		std::vector<uint8_t> bytes;
		SYS_TX_T transaction;
	};
	static bool cleaning = false;
	static std::vector<PendingEvent> pendingEvents;

	struct RegistryLock
	{
		RegistryLock() { fenster::platformAcquireMutex(componentsLock); }
		~RegistryLock() { fenster::platformReleaseMutex(componentsLock); }
	};
	struct CleanupLock
	{
		CleanupLock()
		{
			fenster::platformAcquireMutex(readersDrained);
			RegistryLock registry;
			cleaning = true;
		}
		~CleanupLock()
		{
			std::vector<PendingEvent> events;
			{
				RegistryLock registry;
				events.swap(pendingEvents);
				cleaning = false;
			}
			fenster::platformReleaseMutex(readersDrained);
			// A subscriber may need an interface response to drain its message queue.
			for(auto& event : events)
				fenster::platformSendMessage(event.receiver, event.bytes.data(), event.bytes.size(), event.transaction);
		}
	};

	fenster::ComponentId ComponentRegistry::add(Component* component)
	{
		fenster::platformAcquireMutex(componentsLock);
		fenster::ComponentId id = creationId ? std::exchange(creationId, 0) : next_id++;
		components[id] = component;
		component->id = id;
		fenster::platformReleaseMutex(componentsLock);
		return id;
	}

	fenster::ComponentIdRange ComponentRegistry::reserveIds(SYS_TID_T process)
	{
		RegistryLock registry;
		constexpr auto count = fenster::G_UI_COMPONENT_ID_RESERVATION_SIZE;
		if(next_id > std::numeric_limits<fenster::ComponentId>::max() - count)
			return {};
		auto first = next_id;
		next_id += count;
		reservations.emplace(first, ReservedIds{process, {}});
		return {first, count};
	}

	Component* ComponentRegistry::createReserved(SYS_TID_T process, fenster::ComponentId id,
	                                             const std::function<Component*()>& create)
	{
		{
			RegistryLock registry;
			auto range = reservations.upper_bound(id);
			if(range == reservations.begin()) return nullptr;
			--range;
			auto index = uint64_t(id) - range->first;
			if(range->second.owner != process || index >= range->second.used.size() || range->second.used[index])
				return nullptr;
			range->second.used.set(index); // IDs cannot be reused, including after destruction.
		}
		struct RestoreId
		{
			fenster::ComponentId previous;
			~RestoreId() { creationId = previous; }
		} restore{creationId};
		creationId = id; // Only the root constructor consumes it; internal children allocate normally.
		auto component = create();
		if(component) bind(process, component);
		return component;
	}

	void ComponentRegistry::bind(SYS_TID_T process, Component* component)
	{
		fenster::platformAcquireMutex(componentsLock);
		components_by_process[process][component->id] = component;
		fenster::platformReleaseMutex(componentsLock);
	}

	Component* ComponentRegistry::get(fenster::ComponentId id)
	{
		fenster::platformAcquireMutex(componentsLock);
		if(components.count(id) > 0)
		{
			Component* comp = components[id];
			fenster::platformReleaseMutex(componentsLock);
			return comp;
		}
		fenster::platformReleaseMutex(componentsLock);
		return nullptr;
	}

	bool ComponentRegistry::owns(SYS_TID_T process, fenster::ComponentId id)
	{
		RegistryLock registry;
		auto owner = components_by_process.find(process);
		return owner != components_by_process.end() && owner->second.count(id) != 0;
	}

	ComponentRegistry::Access::Access()
	{
		fenster::platformAcquireMutex(readersLock);
		if(++readers == 1)
			fenster::platformAcquireMutex(readersDrained);
		fenster::platformReleaseMutex(readersLock);
	}
	ComponentRegistry::Access::~Access() { release(); }
	void ComponentRegistry::Access::release()
	{
		if(locked)
		{
			locked = false;
			fenster::platformAcquireMutex(readersLock);
			if(--readers == 0)
				fenster::platformReleaseMutex(readersDrained);
			fenster::platformReleaseMutex(readersLock);
		}
	}

	void ComponentRegistry::sendEvent(SYS_TID_T receiver, void* bytes, size_t size, SYS_TX_T transaction)
	{
		{
			RegistryLock registry;
			if(cleaning)
			{
				PendingEvent event{receiver, std::vector<uint8_t>(size), transaction};
				std::memcpy(event.bytes.data(), bytes, size);
				pendingEvents.push_back(std::move(event));
				return;
			}
		}
		fenster::platformSendMessage(receiver, bytes, size, transaction);
	}

	void ComponentRegistry::forget(fenster::ComponentId id)
	{
		RegistryLock registry;
		MouseGrab::releaseComponent(id);
		components.erase(id);
		for(auto& process : components_by_process)
			process.second.erase(id);
	}

	void ComponentRegistry::destroy(Component* component)
	{
		RegistryLock registry;
		component->setVisible(false);
		// Detach before destruction while the derived type can still send window events.
		if(auto parent = component->getParent())
			parent->removeChild(component);
		delete component;
	}

	void ComponentRegistry::cleanupProcess(SYS_TID_T pid)
	{
		// Readers keep running while cleanup waits, so a UI callback may still
		// receive its interface response. Stop admission only once they have drained.
		CleanupLock cleanup;
		RegistryLock registry;
		MouseGrab::releaseProcess(pid);
		ProcessRegistry::cleanup_process(pid);
		for(auto entry = reservations.begin(); entry != reservations.end();)
			if(entry->second.owner == pid) entry = reservations.erase(entry);
			else ++entry;
		// Shared objects such as Screen can outlive the client that subscribed to them.
		for(auto& component : components)
			component.second->removeListenersForProcess(pid);
		auto process = components_by_process.find(pid);
		if(process == components_by_process.end())
			return;
		// Destructors erase both the root and its owned descendants from this map.
		// Re-read it after each deletion; detached/orphan components belong here too.
		while(!process->second.empty())
			destroy(process->second.begin()->second);
		components_by_process.erase(process);
	}
}

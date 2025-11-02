// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>
#include <map>

#include "component_registry.hpp"

namespace fensterserver
{
	static std::map<fenster::ComponentId, Component*> components;
	static SYS_MUTEX_T componentsLock = fenster::platformInitializeMutex(true);
	static std::map<SYS_TID_T, std::map<fenster::ComponentId, Component*>> components_by_process;
	static fenster::ComponentId next_id = 1;

	fenster::ComponentId ComponentRegistry::add(Component* component)
	{
		fenster::platformAcquireMutex(componentsLock);
		fenster::ComponentId id = next_id++;
		components[id] = component;
		component->id = id;
		fenster::platformReleaseMutex(componentsLock);
		return id;
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

	void ComponentRegistry::removeComponent(SYS_TID_T pid, fenster::ComponentId id)
	{
		fenster::platformAcquireMutex(componentsLock);
		if(components.count(id) > 0)
		{
			if(components_by_process.count(pid) > 0)
			{
				components_by_process[pid].erase(id);
			}
			components.erase(id);
		}
		fenster::platformReleaseMutex(componentsLock);
	}

	void ComponentRegistry::cleanupProcess(SYS_TID_T pid)
	{
		// Get components mapped for process
		fenster::platformAcquireMutex(componentsLock);
		auto components = &components_by_process[pid];

		if(components)
		{
			// Put them into a list
			auto componentList = std::list<Component*>();

			for(auto& entry: *components)
			{
				Component* component = entry.second;
				if(component && std::find(componentList.begin(), componentList.end(), component) == componentList.end())
				{
					componentList.push_back(component);
				}
			}

			// Process items of the list
			std::list<Component*> removedComponents;

			while(!componentList.empty())
			{
				auto component = componentList.back();
				componentList.pop_back();

				if(component->isWindow())
				{
					removeProcessComponents(pid, component, removedComponents);

					for(auto removed: removedComponents)
					{
						componentList.remove(removed);
					}
				}
			}

			// Remove map from registry
			components_by_process.erase(pid);
		}
		fenster::platformReleaseMutex(componentsLock);
	}

	void ComponentRegistry::removeProcessComponents(SYS_TID_T process, Component* component,
	                                                std::list<Component*>& removedComponents)
	{
		// Never remove twice
		if(std::find(removedComponents.begin(), removedComponents.end(), component) != removedComponents.end())
		{
			return;
		}

		// Hide it
		component->setVisible(false);

		// Remove this component
		removedComponents.push_back(component);

		// Recursively remove all child elements first
		auto children = component->acquireChildren();
		for(auto& childRef: children)
		{
			removeProcessComponents(process, childRef.component, removedComponents);
		}
		component->releaseChildren();

		// Remove from registry
		removeComponent(process, component->id);

		// Remove from parent
		auto parent = component->getParent();
		bool canBeDeleted = true;
		if(parent)
		{
			// Only allow to really "delete" children that are default referenced
			ComponentChildReference childReference;
			if(parent->getChildReference(component, childReference))
			{
				canBeDeleted = (childReference.type == COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT);
			}

			// Remove from parent
			parent->removeChild(component);
		}

		// Finally, delete it
		if(canBeDeleted)
		{
			// TODO This results in some endless loop in another thread:
			// delete component;
		}
	}
}

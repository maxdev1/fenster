// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>
#include <map>

#include "component_registry.hpp"

static std::map<g_ui_component_id, component_t*> components;
static SYS_MUTEX_T componentsLock = platformInitializeMutex(true);
static std::map<SYS_TID_T, std::map<g_ui_component_id, component_t*>> components_by_process;
static g_ui_component_id next_id = 1;

g_ui_component_id component_registry_t::add(component_t* component)
{
	platformAcquireMutex(componentsLock);
	g_ui_component_id id = next_id++;
	components[id] = component;
	component->id = id;
	platformReleaseMutex(componentsLock);
	return id;
}

void component_registry_t::bind(SYS_TID_T process, component_t* component)
{
	platformAcquireMutex(componentsLock);
	components_by_process[process][component->id] = component;
	platformReleaseMutex(componentsLock);
}

component_t* component_registry_t::get(g_ui_component_id id)
{
	platformAcquireMutex(componentsLock);
	if(components.count(id) > 0)
	{
		component_t* comp = components[id];
		platformReleaseMutex(componentsLock);
		return comp;
	}
	platformReleaseMutex(componentsLock);
	return nullptr;
}

void component_registry_t::removeComponent(SYS_TID_T pid, g_ui_component_id id)
{
	platformAcquireMutex(componentsLock);
	if(components.count(id) > 0)
	{
		if(components_by_process.count(pid) > 0)
		{
			components_by_process[pid].erase(id);
		}
		components.erase(id);
	}
	platformReleaseMutex(componentsLock);
}

void component_registry_t::cleanupProcess(SYS_TID_T pid)
{
	// Get components mapped for process
	platformAcquireMutex(componentsLock);
	auto components = &components_by_process[pid];

	if(components)
	{
		// Put them into a list
		auto componentList = std::list<component_t*>();

		for(auto& entry: *components)
		{
			component_t* component = entry.second;
			if(component && std::find(componentList.begin(), componentList.end(), component) == componentList.end())
			{
				componentList.push_back(component);
			}
		}

		// Process items of the list
		std::list<component_t*> removedComponents;

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
	platformReleaseMutex(componentsLock);
}

void component_registry_t::removeProcessComponents(SYS_TID_T process, component_t* component,
                                                   std::list<component_t*>& removedComponents)
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
		component_child_reference_t childReference;
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

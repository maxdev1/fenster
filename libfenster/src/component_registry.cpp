// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/component_registry.hpp"
#include "libfenster/components/component.hpp"
#include <map>

namespace fenster
{
	static SYS_MUTEX_T componentsLock = platformInitializeMutex(false);
	static std::map<ComponentId, Component*> components;

	/**
	 *
	 */
	void ComponentRegistry::add(Component* component)
	{
		platformAcquireMutex(componentsLock);
		components[component->getId()] = component;
		platformReleaseMutex(componentsLock);
	}

	/**
	 *
	 */
	Component* ComponentRegistry::get(ComponentId id)
	{
		platformAcquireMutex(componentsLock);

		Component* component = nullptr;
		if(components.count(id) > 0)
			component = components[id];

		platformReleaseMutex(componentsLock);

		return component;
	}
}

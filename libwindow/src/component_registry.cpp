// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/component_registry.hpp"
#include "libwindow/component.hpp"
#include <map>

static SYS_MUTEX_T componentsLock = platformInitializeMutex(false);
static std::map<g_ui_component_id, g_component*> components;

/**
 *
 */
void g_component_registry::add(g_component* component)
{
	platformAcquireMutex(componentsLock);
	components[component->getId()] = component;
	platformReleaseMutex(componentsLock);
}

/**
 *
 */
g_component* g_component_registry::get(g_ui_component_id id)
{
	platformAcquireMutex(componentsLock);

	g_component* component = nullptr;
	if(components.count(id) > 0)
		component = components[id];

	platformReleaseMutex(componentsLock);

	return component;
}

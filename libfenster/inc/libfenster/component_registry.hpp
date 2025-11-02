// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_COMPONENT_REGISTRY
#define LIBFENSTER_COMPONENT_REGISTRY

#include "libfenster/interface.hpp"

namespace fenster
{
	// forward declaration
	class Component;

	class ComponentRegistry
	{
	public:
		static void add(Component *component);
		static Component *get(ComponentId id);
	};
}

#endif

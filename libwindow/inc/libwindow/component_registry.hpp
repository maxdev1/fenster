// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_COMPONENT_REGISTRY
#define LIBWINDOW_COMPONENT_REGISTRY

#include "libwindow/interface.hpp"

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

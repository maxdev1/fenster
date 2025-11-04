// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_LAYOUT
#define LIBFENSTER_LAYOUT_LAYOUT

#include "libfenster/metrics/insets.hpp"
#include "libfenster/interface.hpp"

namespace fenster
{
	class Component;

	class Layout
	{
	protected:
		Component* component;

		explicit Layout(Component* component):
			component(component)
		{
		}

	public:
		virtual ~Layout() = default;

		virtual LayoutType getType() = 0;
	};
}

#endif

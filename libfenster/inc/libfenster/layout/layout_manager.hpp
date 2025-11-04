// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_LAYOUTMANAGER
#define LIBFENSTER_LAYOUT_LAYOUTMANAGER

#include "libfenster/metrics/insets.hpp"
#include "libfenster/interface.hpp"

namespace fenster
{
	class Component;

	class LayoutManager
	{
	protected:
		Component* component;

		explicit LayoutManager(Component* component):
			component(component)
		{
		}

	public:
		virtual ~LayoutManager() = default;

		virtual void setPadding(fenster::Insets insets)
		{
		}

		virtual LayoutManagerType getType() = 0;
	};
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_ORIENTATIONSUPPORT
#define LIBFENSTER_LAYOUT_ORIENTATIONSUPPORT

#include "libfenster/layout/layout.hpp"

namespace fenster
{
	enum class Orientation : uint32_t
	{
		Horizontal = 0,
		Vertical = 1,
	};

	/**
	 * Extension to layouts that adds support for layout orientation.
	 */
	class OrientationSupport
	{
		Component* component;

	protected:
		explicit OrientationSupport(Component* component):
			component(component)
		{
		}

	public:
		virtual ~OrientationSupport() = default;

		virtual void setOrientation(Orientation orientation);
	};
}

#endif

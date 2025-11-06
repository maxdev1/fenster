// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_SINGLESPACINGSUPPORT
#define LIBFENSTER_LAYOUT_SINGLESPACINGSUPPORT

#include "libfenster/layout/layout.hpp"

namespace fenster
{
	/**
	 * Extension to layouts that adds support for a single space value.
	 */
	class SingleSpacingSupport
	{
		Component* component;

	protected:
		explicit SingleSpacingSupport(Component* component):
			component(component)
		{
		}

	public:
		virtual ~SingleSpacingSupport() = default;

		virtual void setSpace(int space);
	};
}

#endif

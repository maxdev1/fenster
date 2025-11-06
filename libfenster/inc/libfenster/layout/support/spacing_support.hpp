// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_SPACINGSUPPORT
#define LIBFENSTER_LAYOUT_SPACINGSUPPORT

#include "libfenster/layout/layout.hpp"

namespace fenster
{
	/**
	 * Extension to layouts that adds support for horizontal and vertical spacing.
	 */
	class SpacingSupport
	{
		Component* component;

	protected:
		explicit SpacingSupport(Component* component):
			component(component)
		{
		}

	public:
		virtual ~SpacingSupport() = default;

		virtual void setHorizontalSpace(int space);
		virtual void setVerticalSpace(int space);
	};
}

#endif

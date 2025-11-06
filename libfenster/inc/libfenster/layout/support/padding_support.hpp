// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_PADDINGSUPPORT
#define LIBFENSTER_LAYOUT_PADDINGSUPPORT

#include "libfenster/layout/layout.hpp"

namespace fenster
{
	/**
	 * Extension to layouts that adds support for padding.
	 */
	class PaddingSupport
	{
		Component* component;

	protected:
		explicit PaddingSupport(Component* component):
			component(component)
		{
		}

	public:
		virtual ~PaddingSupport() = default;

		virtual void setPadding(Insets insets);
	};
}

#endif

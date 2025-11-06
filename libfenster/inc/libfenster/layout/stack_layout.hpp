// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_STACKLAYOUT
#define LIBFENSTER_LAYOUT_STACKLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/single_spacing_support.hpp"
#include "support/orientation_support.hpp"

namespace fenster
{
	class StackLayout : public Layout, public PaddingSupport, public SingleSpacingSupport, public OrientationSupport
	{
	protected:
		explicit StackLayout(Component* component):
			Layout(component), PaddingSupport(component), SingleSpacingSupport(component), OrientationSupport(component)
		{
		}

	public:
		static StackLayout* create(Component* component);

		LayoutType getType() override
		{
			return FENSTER_LAYOUT_STACK;
		}
	};
}

#endif

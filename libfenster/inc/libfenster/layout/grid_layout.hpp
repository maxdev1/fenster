// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_GRIDLAYOUT
#define LIBFENSTER_LAYOUT_GRIDLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/spacing_support.hpp"

namespace fenster
{
	class GridLayout : public Layout, public PaddingSupport, public SpacingSupport
	{
	protected:
		explicit GridLayout(Component* component):
			Layout(component), PaddingSupport(component), SpacingSupport(component)
		{
		}

	public:
		static GridLayout* create(Component* component);

		LayoutType getType() override
		{
			return LayoutType::Grid;
		}
	};
}

#endif

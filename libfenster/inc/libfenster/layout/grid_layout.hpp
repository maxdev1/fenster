// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_GRIDLAYOUT
#define LIBFENSTER_LAYOUT_GRIDLAYOUT

#include "layout.hpp"

namespace fenster
{
	class GridLayout : public Layout
	{
	protected:
		explicit GridLayout(Component* component):
			Layout(component)
		{
		}

	public:
		static GridLayout* create(Component* component);

		LayoutType getType() override
		{
			return FENSTER_LAYOUT_GRID;
		}
	};
}

#endif

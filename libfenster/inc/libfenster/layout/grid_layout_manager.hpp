// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_GRIDLAYOUTMANAGER
#define LIBFENSTER_LAYOUT_GRIDLAYOUTMANAGER

#include "layout_manager.hpp"

namespace fenster
{
	/**
	 *
	 */
	class GridLayoutManager : public LayoutManager
	{
	protected:
		explicit GridLayoutManager(Component* component):
			LayoutManager(component)
		{
		}

	public:
		static GridLayoutManager* create(Component* component);

		LayoutManagerType getType() override
		{
			return FENSTER_LAYOUT_MANAGER_GRID;
		}
	};
}

#endif

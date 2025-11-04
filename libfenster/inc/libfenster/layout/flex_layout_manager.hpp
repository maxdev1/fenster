// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FLEXLAYOUTMANAGER
#define LIBFENSTER_LAYOUT_FLEXLAYOUTMANAGER

#include "layout_manager.hpp"

namespace fenster
{
	/**
	 *
	 */
	class FlexLayoutManager : public LayoutManager
	{
	protected:
		explicit FlexLayoutManager(Component* component):
			LayoutManager(component)
		{
		}

	public:
		void setPadding(Insets insets) override;
		void setSpace(int space);
		void setHorizontal(bool horizontal);
		void setComponentInfo(Component* component, float grow, float shrink, int basis);

		static FlexLayoutManager* create(Component* component);

		LayoutManagerType getType() override
		{
			return FENSTER_LAYOUT_MANAGER_FLEX;
		}
	};
}

#endif

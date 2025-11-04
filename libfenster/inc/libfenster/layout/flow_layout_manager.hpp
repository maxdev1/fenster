// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FLOWLAYOUTMANAGER
#define LIBFENSTER_LAYOUT_FLOWLAYOUTMANAGER

#include "layout_manager.hpp"

namespace fenster
{
	/**
	 *
	 */
	class FlowLayoutManager : public LayoutManager
	{
	protected:
		explicit FlowLayoutManager(Component* component):
			LayoutManager(component)
		{
		}

	public:
		void setPadding(fenster::Insets insets) override;

		static FlowLayoutManager* create(Component* component);

		LayoutManagerType getType() override
		{
			return FENSTER_LAYOUT_MANAGER_FLOW;
		}
	};
}

#endif

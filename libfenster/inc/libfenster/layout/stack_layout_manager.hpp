// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_STACKLAYOUTMANAGER
#define LIBFENSTER_LAYOUT_STACKLAYOUTMANAGER

#include "layout_manager.hpp"

namespace fenster
{
	/**
	 *
	 */
	class StackLayoutManager : public LayoutManager
	{
	protected:
		explicit StackLayoutManager(Component* component):
			LayoutManager(component)
		{
		}

	public:
		void setPadding(fenster::Insets insets) override;
		void setSpace(int space);
		void setHorizontal(bool horizontal);

		static StackLayoutManager* create(Component* component);

		LayoutManagerType getType() override
		{
			return FENSTER_LAYOUT_MANAGER_STACK;
		}
	};
}

#endif

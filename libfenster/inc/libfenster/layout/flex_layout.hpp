// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FLEXLAYOUT
#define LIBFENSTER_LAYOUT_FLEXLAYOUT

#include "layout.hpp"

namespace fenster
{
	class FlexLayout : public Layout
	{
	protected:
		explicit FlexLayout(Component* component):
			Layout(component)
		{
		}

	public:
		void setPadding(Insets insets);
		void setSpace(int space);
		void setHorizontal(bool horizontal);
		void setComponentInfo(Component* component, float grow, float shrink, int basis);

		static FlexLayout* create(Component* component);

		LayoutType getType() override
		{
			return FENSTER_LAYOUT_FLEX;
		}
	};
}

#endif

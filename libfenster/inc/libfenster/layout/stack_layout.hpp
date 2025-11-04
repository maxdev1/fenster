// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_STACKLAYOUT
#define LIBFENSTER_LAYOUT_STACKLAYOUT

#include "layout.hpp"

namespace fenster
{
	class StackLayout : public Layout
	{
	protected:
		explicit StackLayout(Component* component):
			Layout(component)
		{
		}

	public:
		void setPadding(Insets insets);
		void setSpace(int space);
		void setHorizontal(bool horizontal);

		static StackLayout* create(Component* component);

		LayoutType getType() override
		{
			return FENSTER_LAYOUT_STACK;
		}
	};
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FLOWLAYOUT
#define LIBFENSTER_LAYOUT_FLOWLAYOUT

#include "layout.hpp"

namespace fenster
{
	class FlowLayout : public Layout
	{
	protected:
		explicit FlowLayout(Component* component):
			Layout(component)
		{
		}

	public:
		void setPadding(fenster::Insets insets);

		static FlowLayout* create(Component* component);

		LayoutType getType() override
		{
			return FENSTER_LAYOUT_FLOW;
		}
	};
}

#endif

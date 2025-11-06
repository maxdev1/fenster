// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FLOWLAYOUT
#define LIBFENSTER_LAYOUT_FLOWLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/single_spacing_support.hpp"

namespace fenster
{
	class FlowLayout : public Layout, public PaddingSupport, public SingleSpacingSupport
	{
	protected:
		explicit FlowLayout(Component* component):
			Layout(component), PaddingSupport(component), SingleSpacingSupport(component)
		{
		}

	public:
		static FlowLayout* create(Component* component);

		LayoutType getType() override
		{
			return LayoutType::Flow;
		}
	};
}

#endif

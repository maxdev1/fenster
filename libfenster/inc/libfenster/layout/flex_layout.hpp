// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FLEXLAYOUT
#define LIBFENSTER_LAYOUT_FLEXLAYOUT

#include "layout.hpp"
#include "support/orientation_support.hpp"
#include "support/padding_support.hpp"
#include "support/single_spacing_support.hpp"

namespace fenster
{
	class FlexLayout : public Layout, public PaddingSupport, public SingleSpacingSupport, public OrientationSupport
	{
	protected:
		explicit FlexLayout(Component* component):
			Layout(component), PaddingSupport(component), SingleSpacingSupport(component), OrientationSupport(component)
		{
		}

	public:
		void setComponentInfo(Component* component, float grow, float shrink, int basis);

		static FlexLayout* create(Component* component);

		LayoutType getType() override
		{
			return LayoutType::Flex;
		}
	};
}

#endif

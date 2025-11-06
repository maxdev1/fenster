// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/support/spacing_support.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/component.hpp"

namespace fenster
{
	void SpacingSupport::setHorizontalSpace(int space)
	{
		component->setNumericProperty(ComponentProperty::LayoutSpaceHorizontal, space);
	}

	void SpacingSupport::setVerticalSpace(int space)
	{
		component->setNumericProperty(ComponentProperty::LayoutSpaceVertical, space);
	}
}

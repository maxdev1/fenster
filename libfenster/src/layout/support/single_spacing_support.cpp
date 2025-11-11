// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/support/single_spacing_support.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/components/component.hpp"

namespace fenster
{
	void SingleSpacingSupport::setSpace(int space)
	{
		component->setNumericProperty(ComponentProperty::LayoutSpace, space);
	}
}

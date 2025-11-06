// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/support/orientation_support.hpp"
#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	void OrientationSupport::setOrientation(Orientation orientation)
	{
		component->setNumericProperty(ComponentProperty::LayoutOrientation, (uint32_t) orientation);
	}
}

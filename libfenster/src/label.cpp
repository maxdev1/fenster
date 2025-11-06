// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/label.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	Label* Label::create()
	{
		return createComponent<Label, FENSTER_COMPONENT_TYPE_LABEL>();
	}

	bool Label::setColor(ColorArgb argb)
	{
		return setNumericProperty(ComponentProperty::Color, argb);
	}

	bool Label::setAlignment(TextAlignment alignment)
	{
		return setNumericProperty(ComponentProperty::Alignment, (uint32_t) alignment);
	}
}

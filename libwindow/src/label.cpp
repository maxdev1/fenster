// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/label.hpp"
#include "libwindow/properties.hpp"

namespace fenster
{
	Label* Label::create()
	{
		return createComponent<Label, FENSTER_COMPONENT_TYPE_LABEL>();
	}

	bool Label::setColor(ColorArgb argb)
	{
		return setNumericProperty(FENSTER_UI_PROPERTY_COLOR, argb);
	}

	bool Label::setAlignment(TextAlignment alignment)
	{
		return setNumericProperty(FENSTER_UI_PROPERTY_ALIGNMENT, (uint32_t) alignment);
	}
}

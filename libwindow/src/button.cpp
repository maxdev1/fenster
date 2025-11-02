// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/button.hpp"
#include "libwindow/component.hpp"
#include "libwindow/properties.hpp"

namespace fenster
{
	Button* Button::create()
	{
		return createComponent<Button, FENSTER_COMPONENT_TYPE_BUTTON>();
	}

	void Button::setEnabled(bool enabled)
	{
		setNumericProperty(FENSTER_UI_PROPERTY_ENABLED, enabled);
	}

	bool Button::isEnabled()
	{
		uint32_t value;
		getNumericProperty(FENSTER_UI_PROPERTY_ENABLED, &value);
		return value;
	}

	void Button::setStyle(ButtonStyle style)
	{
		setNumericProperty(FENSTER_UI_PROPERTY_STYLE, style);
	}

	ButtonStyle Button::getStyle()
	{
		uint32_t value;
		getNumericProperty(FENSTER_UI_PROPERTY_STYLE, &value);
		return value;
	}
}

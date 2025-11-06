// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/button.hpp"
#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	Button* Button::create()
	{
		return createComponent<Button, ComponentType::Button>();
	}

	void Button::setEnabled(bool enabled)
	{
		setNumericProperty(ComponentProperty::Enabled, enabled);
	}

	bool Button::isEnabled()
	{
		uint32_t value;
		getNumericProperty(ComponentProperty::Enabled, &value);
		return value;
	}

	void Button::setVariant(ButtonVariant variant)
	{
		setNumericProperty(ComponentProperty::Variant, (uint32_t) variant);
	}

	ButtonVariant Button::getVariant()
	{
		uint32_t value;
		getNumericProperty(ComponentProperty::Variant, &value);
		return (ButtonVariant) value;
	}
}

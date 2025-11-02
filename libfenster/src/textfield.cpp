// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/textfield.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	TextField* TextField::create()
	{
		return createComponent<TextField, FENSTER_COMPONENT_TYPE_TEXTFIELD>();
	}

	void TextField::setSecure(bool secure)
	{
		Component::setNumericProperty(FENSTER_UI_PROPERTY_SECURE, secure);
	}

	bool TextField::isSecure()
	{
		uint32_t value;
		Component::getNumericProperty(FENSTER_UI_PROPERTY_SECURE, &value);
		return value;
	}
}


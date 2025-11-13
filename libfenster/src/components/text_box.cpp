// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/components/text_box.hpp"

namespace fenster
{
	TextBox* TextBox::create()
	{
		return createComponent<TextBox, ComponentType::TextBox>();
	}

	void TextBox::setSecure(bool secure)
	{
		Component::setNumericProperty(ComponentProperty::Secure, secure);
	}

	bool TextBox::isSecure()
	{
		uint32_t value;
		Component::getNumericProperty(ComponentProperty::Secure, &value);
		return value;
	}
}

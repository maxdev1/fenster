// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/textfield.hpp"
#include "libwindow/properties.hpp"

g_textfield* g_textfield::create()
{
	return createComponent<g_textfield, G_UI_COMPONENT_TYPE_TEXTFIELD>();
}

void g_textfield::setSecure(bool secure)
{
	g_component::setNumericProperty(G_UI_PROPERTY_SECURE, secure);
}

bool g_textfield::isSecure()
{
	uint32_t value;
	g_component::getNumericProperty(G_UI_PROPERTY_SECURE, &value);
	return value;
}

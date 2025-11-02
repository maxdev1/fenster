// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/button.hpp"
#include "libwindow/component.hpp"
#include "libwindow/properties.hpp"

g_button* g_button::create()
{
	return createComponent<g_button, G_UI_COMPONENT_TYPE_BUTTON>();
}

void g_button::setEnabled(bool enabled)
{
	setNumericProperty(G_UI_PROPERTY_ENABLED, enabled);
}

bool g_button::isEnabled()
{
	uint32_t value;
	getNumericProperty(G_UI_PROPERTY_ENABLED, &value);
	return value;
}

void g_button::setStyle(g_button_style style)
{
	setNumericProperty(G_UI_PROPERTY_STYLE, style);
}

g_button_style g_button::getStyle()
{
	uint32_t value;
	getNumericProperty(G_UI_PROPERTY_STYLE, &value);
	return value;
}



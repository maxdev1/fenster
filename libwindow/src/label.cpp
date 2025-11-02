// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/label.hpp"
#include "libwindow/properties.hpp"

g_label* g_label::create()
{
	return createComponent<g_label, G_UI_COMPONENT_TYPE_LABEL>();
}

bool g_label::setColor(g_color_argb argb)
{
	return setNumericProperty(G_UI_PROPERTY_COLOR, argb);
}

bool g_label::setAlignment(g_text_alignment alignment)
{
	return setNumericProperty(G_UI_PROPERTY_ALIGNMENT, (uint32_t) alignment);
}

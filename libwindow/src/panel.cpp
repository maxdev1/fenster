// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/panel.hpp"

g_panel* g_panel::create()
{
	return createComponent<g_panel, G_UI_COMPONENT_TYPE_PANEL>();
}

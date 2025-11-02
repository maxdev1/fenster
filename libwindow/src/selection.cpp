// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/selection.hpp"

g_selection* g_selection::create()
{
	return createComponent<g_selection, G_UI_COMPONENT_TYPE_SELECTION>();
}

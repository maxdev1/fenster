// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_SCROLLPANE
#define LIBWINDOW_SCROLLPANE

#include "component.hpp"

class g_scrollpane : virtual public g_component
{
public:
    explicit g_scrollpane(g_ui_component_id id) : g_component(id)
    {
    }

    static g_scrollpane* create();

	bool setContent(g_component* content);
	bool setFixed(bool width, bool height);
};

#endif

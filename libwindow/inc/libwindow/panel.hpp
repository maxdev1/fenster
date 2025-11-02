// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_PANEL
#define LIBWINDOW_PANEL

#include "component.hpp"

class g_panel : virtual public g_component
{
public:
    explicit g_panel(g_ui_component_id id) : g_component(id)
    {
    }

    static g_panel* create();
};

#endif

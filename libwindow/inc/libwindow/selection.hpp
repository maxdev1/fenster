// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_SELECTION
#define LIBWINDOW_SELECTION

#include "component.hpp"

class g_selection : virtual public g_component
{
public:
    explicit g_selection(g_ui_component_id id) : g_component(id)
    {
    }

    static g_selection* create();
};

#endif

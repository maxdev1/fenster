// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_TEXTFIELD
#define LIBWINDOW_TEXTFIELD

#include "component.hpp"
#include "titled_component.hpp"

class g_textfield : virtual public g_component, virtual public g_titled_component
{
public:
    explicit g_textfield(g_ui_component_id id) : g_component(id), g_titled_component(id)
    {
    }

    static g_textfield* create();

    void setSecure(bool secure);
    bool isSecure();
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_BUTTON
#define LIBWINDOW_BUTTON

#include "action_component.hpp"
#include "component.hpp"
#include "titled_component.hpp"

typedef uint8_t g_button_style;
#define G_BUTTON_STYLE_DEFAULT  0
#define G_BUTTON_STYLE_GHOST    1

class g_button : virtual public g_component, virtual public g_titled_component, virtual public g_action_component
{
public:
    explicit g_button(g_ui_component_id id) : g_component(id), g_titled_component(id), g_action_component(id)
    {
    }

    static g_button* create();

    void setEnabled(bool enabled);
    bool isEnabled();

    void setStyle(g_button_style style);
    g_button_style getStyle();
};

#endif

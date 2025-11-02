// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_LABEL
#define LIBWINDOW_LABEL

#include "component.hpp"
#include "titled_component.hpp"
#include "libwindow/font/text_alignment.hpp"

class g_label : virtual public g_component, virtual public g_titled_component
{
public:
    explicit g_label(g_ui_component_id id) :
        g_component(id), g_titled_component(id)
    {
    }

    static g_label* create();

    bool setColor(g_color_argb argb);
    bool setAlignment(g_text_alignment alignment);
};

#endif

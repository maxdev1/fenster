// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_WINDOW
#define LIBWINDOW_WINDOW

#include "component.hpp"
#include "focusable_component.hpp"
#include "titled_component.hpp"

class g_window :
    virtual public g_component,
    virtual public g_titled_component,
    virtual public g_focusable_component
{
public:
    explicit g_window(g_ui_component_id id):
        g_component(id), g_titled_component(id), g_focusable_component(id)
    {
    }

    static g_window* create();
    static g_window* attach(g_ui_component_id id);

    bool isResizable();
    void setResizable(bool resizable);

    bool onClose(std::function<void()> func);
};

#endif

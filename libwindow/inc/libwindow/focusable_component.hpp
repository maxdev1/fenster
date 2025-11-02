// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_FOCUSABLE_COMPONENT__
#define __LIBWINDOW_FOCUSABLE_COMPONENT__

#include <bits/std_function.h>

#include "interface.hpp"
#include "component.hpp"

class g_focusable_component : virtual public g_component
{
protected:
    explicit g_focusable_component(g_ui_component_id id) : g_component(id)
    {
    }

public:
    ~g_focusable_component() override = default;

    virtual bool setFocused(bool focused);
    virtual bool isFocused();

    virtual void addFocusListener(std::function<void(bool)> callback);
};

#endif

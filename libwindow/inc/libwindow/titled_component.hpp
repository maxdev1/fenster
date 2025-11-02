// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_TITLED_COMPONENT__
#define __LIBWINDOW_TITLED_COMPONENT__

#include <bits/std_function.h>

#include "interface.hpp"
#include "component.hpp"

class g_titled_component : virtual public g_component
{
protected:
    explicit g_titled_component(g_ui_component_id id) : g_component(id)
    {
    }

public:
    ~g_titled_component() override = default;

    virtual bool setTitle(std::string title);
    virtual std::string getTitle();

    virtual void addTitleListener(std::function<void(std::string)> callback);
};

#endif

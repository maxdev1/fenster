// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_IMAGE
#define LIBWINDOW_IMAGE

#include "component.hpp"

class g_image : virtual public g_component
{
public:
    explicit g_image(g_ui_component_id id) :
        g_component(id)
    {
    }

    static g_image* create();

	bool loadImage(std::string path);
};

#endif

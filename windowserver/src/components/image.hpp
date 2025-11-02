// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_IMAGE__
#define __WINDOWSERVER_COMPONENTS_IMAGE__

#include "components/component.hpp"
#include "components/titled_component.hpp"

#include <libwindow/font/font.hpp>

class image_t : virtual public component_t
{
    cairo_surface_t* image = nullptr;

public:
    image_t();
    ~image_t() override = default;

    void paint() override;

    void loadImage(std::string path);

    bool setStringProperty(int property, std::string text) override;
};

#endif

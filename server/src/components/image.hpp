// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_IMAGE
#define FENSTER_SERVER_COMPONENTS_IMAGE

#include "components/component.hpp"
#include "components/titled_component.hpp"

#include <libfenster/font/font.hpp>

namespace fensterserver
{
    class Image : virtual public Component
    {
        cairo_surface_t* image = nullptr;

    public:
        Image();
        ~Image() override = default;

        void paint() override;

        void loadImage(std::string path);

        bool setStringProperty(int property, std::string text) override;
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_IMAGE
#define FENSTER_SERVER_COMPONENTS_IMAGE

#include "components/component.hpp"
#include "components/titled_component.hpp"

#include <libfenster/font/font.hpp>
#include <libfenster/icons.hpp>

namespace fensterserver
{
    class Image : virtual public Component
    {
        cairo_surface_t* image = nullptr;
        fenster::Icon icon = fenster::Icon::None;
		std::string imagePath;

    public:
        Image();
        ~Image() override;

        void paint() override;
        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;

        void loadImage(std::string path);

        bool setStringProperty(fenster::ComponentProperty property, std::string text) override;
    };
}

#endif

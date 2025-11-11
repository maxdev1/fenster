// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_MENUBAR
#define FENSTER_SERVER_COMPONENTS_MENUBAR

#include "component.hpp"
#include <libfenster/color_argb.hpp>

namespace fensterserver
{
    class MenuBar : virtual public Component
    {
        fenster::ColorArgb background;

    protected:
        bool hasGraphics() const override
        {
            return background != ARGB(0, 0, 0, 0);
        }

    public:
        MenuBar();

        void paint() override;

        void setBackground(fenster::ColorArgb color);
        fenster::ColorArgb getBackground();

        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;
        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;
    };
}

#endif

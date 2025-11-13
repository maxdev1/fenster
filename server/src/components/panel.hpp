// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_PANEL
#define FENSTER_SERVER_COMPONENTS_PANEL

#include "components/component.hpp"
#include "layout/stack_layout.hpp"
#include <libfenster/color_argb.hpp>

namespace fensterserver
{
    class Panel : virtual public Component
    {
        fenster::ColorArgb background;

    protected:
        bool hasGraphics() const override
        {
            return background != ARGB(0, 0, 0, 0);
        }

    public:
        Panel() : background(ARGB(0, 0, 0, 0))
        {
        }

        void paint() override;

        void setBackground(fenster::ColorArgb color);
        fenster::ColorArgb getBackground();

        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;
        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;
    };
}

#endif

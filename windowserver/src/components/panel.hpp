// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_PANEL__
#define __WINDOWSERVER_COMPONENTS_PANEL__

#include "components/component.hpp"
#include <libwindow/color_argb.hpp>

class panel_t : virtual public component_t
{
    g_color_argb background;

protected:
    bool hasGraphics() const override
    {
        return background != ARGB(0, 0, 0, 0);
    }

public:
    panel_t() : background(ARGB(0, 0, 0, 0))
    {
    }

    void paint() override;

    void setBackground(g_color_argb color);
    g_color_argb getBackground();

    bool setNumericProperty(int property, uint32_t value) override;
    bool getNumericProperty(int property, uint32_t* out) override;
};

#endif

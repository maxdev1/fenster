// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_LABEL__
#define __WINDOWSERVER_COMPONENTS_LABEL__

#include "components/component.hpp"
#include "components/titled_component.hpp"

#include <libwindow/font/font.hpp>
#include <libwindow/font/text_alignment.hpp>
#include <libwindow/color_argb.hpp>

class label_t : virtual public component_t, virtual public titled_component_t
{
    g_font* font;
    int fontSize;
    cairo_text_extents_t textExtents;
    cairo_font_extents_t fontExtents;

    std::string text;
    g_text_alignment alignment;
    g_color_argb color;

public:
    label_t();
    ~label_t() override = default;

    void paint() override;
    void update() override;

    virtual void setFont(g_font* font);

    virtual void setColor(g_color_argb color);
    virtual g_color_argb getColor();

    void setTitleInternal(std::string title) override;
    std::string getTitle() override;

    void setAlignment(g_text_alignment alignment);
    g_text_alignment getAlignment();

    bool setNumericProperty(int property, uint32_t value) override;
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_LABEL
#define FENSTER_SERVER_COMPONENTS_LABEL

#include "components/component.hpp"
#include "components/titled_component.hpp"

#include <libwindow/font/font.hpp>
#include <libwindow/font/text_alignment.hpp>
#include <libwindow/color_argb.hpp>

namespace fensterserver
{
    class Label : virtual public Component, virtual public TitledComponent
    {
        fenster::Font* font;
        int fontSize;
        cairo_text_extents_t textExtents;
        cairo_font_extents_t fontExtents;

        std::string text;
        fenster::TextAlignment alignment;
        fenster::ColorArgb color;

    public:
        Label();
        ~Label() override = default;

        void paint() override;
        void update() override;

        virtual void setFont(fenster::Font* font);

        virtual void setColor(fenster::ColorArgb color);
        virtual fenster::ColorArgb getColor();

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        void setAlignment(fenster::TextAlignment alignment);
        fenster::TextAlignment getAlignment();

        bool setNumericProperty(int property, uint32_t value) override;
    };
}

#endif

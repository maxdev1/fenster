// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TEXTFIELD
#define FENSTER_SERVER_COMPONENTS_TEXTFIELD

#include "components/text/text_component.hpp"
#include "components/titled_component.hpp"

#include <libfenster/input/key_info.hpp>
#include <libfenster/font/font.hpp>
#include <libfenster/font/text_layouter.hpp>
#include <libfenster/metrics/insets.hpp>
#include <libfenster/color_argb.hpp>
#include <string>

namespace fensterserver
{
    enum class TextFieldVisualStatus : uint8_t
    {
        NORMAL,
        HOVERED
    };

    class TextField : virtual public TextComponent, virtual public TitledComponent
    {
        std::string text;
        TextFieldVisualStatus visualStatus;
        bool secure;

        fenster::Font* font;

        int scrollX;
        int fontSize;
        fenster::ColorArgb textColor;
        fenster::Insets insets;

        int cursor;
        int marker;

        fenster::LayoutedText* viewModel;

        bool shiftDown = false;

        void loadDefaultFont();
        void applyScroll();

    public:
        TextField();
        ~TextField() override = default;

        void update() override;
        void paint() override;

        Component* handleKeyEvent(KeyEvent& e) override;
        Component* handleMouseEvent(MouseEvent& e) override;

        bool getNumericProperty(int property, uint32_t* out) override;
        bool setNumericProperty(int property, uint32_t value) override;

        void setText(std::string text) override;
        std::string getText() override;

        bool isFocusableDefault() const override
        {
            return true;
        }

        void setFocusedInternal(bool focused) override;

        virtual void setSecure(bool secure);

        virtual bool isSecure()
        {
            return secure;
        }

        void setTitleInternal(std::string title) override
        {
            setText(title);
        }

        std::string getTitle() override
        {
            return getText();
        }

        void setCursor(int pos) override;

        int getCursor() override
        {
            return cursor;
        }

        void setMarker(int pos) override;

        int getMarker() override
        {
            return marker;
        }

        fenster::Range getSelectedRange() override;

        void backspace(fenster::KeyInfo& info);

        void insert(std::string text);
        int viewToPosition(fenster::Point p);
        fenster::Rectangle glyphToView(fenster::PositionedGlyph& g);
        int positionToUnscrolledCursorX(int pos);
        fenster::Rectangle positionToCursorBounds(int pos);
        void setFont(fenster::Font* f);
    };
}

#endif

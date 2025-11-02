// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __TEXT_FIELD__
#define __TEXT_FIELD__

#include "components/text/text_component.hpp"
#include "components/titled_component.hpp"

#include <libwindow/font/font.hpp>
#include <libwindow/font/text_layouter.hpp>
#include <libwindow/metrics/insets.hpp>
#include <libwindow/color_argb.hpp>
#include <libwindow/platform/platform-key-info.hpp>
#include <string>

enum class text_field_visual_status_t : uint8_t
{
    NORMAL,
    HOVERED
};

class text_field_t : virtual public text_component_t, virtual public titled_component_t
{
    std::string text;
    text_field_visual_status_t visualStatus;
    bool secure;

    g_font* font;

    int scrollX;
    int fontSize;
    g_color_argb textColor;
    g_insets insets;

    int cursor;
    int marker;

    g_layouted_text* viewModel;

    bool shiftDown = false;

    void loadDefaultFont();
    void applyScroll();

public:
    text_field_t();
    ~text_field_t() override = default;

    void update() override;
    void paint() override;

    component_t* handleKeyEvent(key_event_t& e) override;
    component_t* handleMouseEvent(mouse_event_t& e) override;

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

    g_range getSelectedRange() override;

    void backspace(g_key_info& info);

    void insert(std::string text);
    int viewToPosition(g_point p);
    g_rectangle glyphToView(g_positioned_glyph& g);
    int positionToUnscrolledCursorX(int pos);
    g_rectangle positionToCursorBounds(int pos);
    void setFont(g_font* f);
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_WINDOW__
#define __WINDOWSERVER_COMPONENTS_WINDOW__

#include "components/component.hpp"
#include "components/label.hpp"
#include "components/panel.hpp"
#include "components/titled_component.hpp"

#include <bits/std_function.h>
#include <libwindow/color_argb.hpp>

#include "focusable_component.hpp"

/**
 * constants for border sizes
 */
#define DEFAULT_BORDER_WIDTH 7
#define DEFAULT_CORNER_SIZE 15

/**
 * modes used when resizing windows
 */
enum window_resize_mode_t
{
    RESIZE_MODE_NONE,
    RESIZE_MODE_MOVE,
    RESIZE_MODE_TOP,
    RESIZE_MODE_TOP_RIGHT,
    RESIZE_MODE_RIGHT,
    RESIZE_MODE_BOTTOM_RIGHT,
    RESIZE_MODE_BOTTOM,
    RESIZE_MODE_BOTTOM_LEFT,
    RESIZE_MODE_LEFT,
    RESIZE_MODE_TOP_LEFT
};

/**
 *
 */
class window_t :
    virtual public component_t,
    virtual public titled_component_t
{
    int borderWidth;
    int cornerSize;
    g_color_argb backgroundColor;
    bool resizable;

    label_t label;
    panel_t panel;

    bool crossPressed;
    bool crossHovered;

    g_point pressPoint;
    g_rectangle pressBounds;
    window_resize_mode_t resizeMode;

    int titleHeight;
    int shadowSize;
    g_rectangle crossBounds;

public:
    window_t();

    ~window_t() override = default;

    panel_t* getPanel()
    {
        return &panel;
    }

    void setBackground(g_color_argb color)
    {
        backgroundColor = color;
        markFor(COMPONENT_REQUIREMENT_PAINT);
    }

    g_color_argb getBackground() const
    {
        return backgroundColor;
    }

    bool isWindow() override
    {
        return true;
    }

    void addChild(component_t* component,
                  component_child_reference_type_t type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;

    void layout() override;
    void paint() override;

    component_t* handleMouseEvent(mouse_event_t& e) override;

    bool isFocusableDefault() const override
    {
        return true;
    }

    void setFocusedInternal(bool focused) override;

    bool getNumericProperty(int property, uint32_t* out) override;
    bool setNumericProperty(int property, uint32_t value) override;

    void setTitleInternal(std::string title) override;
    std::string getTitle() override;

    virtual void close();

    void setLayoutManager(layout_manager_t* layoutManager) override;
    layout_manager_t* getLayoutManager() const override;
};

#endif

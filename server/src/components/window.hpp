// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_WINDOW
#define FENSTER_SERVER_COMPONENTS_WINDOW

#include "components/component.hpp"
#include "components/label.hpp"
#include "components/panel.hpp"
#include "components/titled_component.hpp"

#include <bits/std_function.h>
#include <libfenster/color_argb.hpp>

#include "focusable_component.hpp"

/**
 * constants for border sizes
 */
#define DEFAULT_BORDER_WIDTH 7
#define DEFAULT_CORNER_SIZE 15

namespace fensterserver
{
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
    class Window :
        virtual public Component,
        virtual public TitledComponent
    {
        int borderWidth;
        int cornerSize;
        fenster::ColorArgb backgroundColor;
        bool resizable;

        Label label;
        Panel panel;

        bool crossPressed;
        bool crossHovered;

        fenster::Point pressPoint;
        fenster::Rectangle pressBounds;
        window_resize_mode_t resizeMode;

        int titleHeight;
        int shadowSize;
        fenster::Rectangle crossBounds;

    public:
        Window();

        ~Window() override = default;

        Panel* getPanel()
        {
            return &panel;
        }

        void setBackground(fenster::ColorArgb color)
        {
            backgroundColor = color;
            markFor(COMPONENT_REQUIREMENT_PAINT);
        }

        fenster::ColorArgb getBackground() const
        {
            return backgroundColor;
        }

        bool isWindow() override
        {
            return true;
        }

        void addChild(Component* component,
                      ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;

        void layout() override;
        void paint() override;

        Component* handleMouseEvent(MouseEvent& e) override;

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

        void setLayoutManager(LayoutManager* layoutManager) override;
        LayoutManager* getLayoutManager() const override;
    };
}

#endif

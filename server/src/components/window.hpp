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
#include "menu_bar.hpp"

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
    typedef uint16_t WindowResizeMode;
#define RESIZE_MODE_NONE 0b00000000
#define RESIZE_MODE_MOVE 0b00000001
#define RESIZE_MODE_TOP 0b00000010
#define RESIZE_MODE_RIGHT 0b00000100
#define RESIZE_MODE_BOTTOM 0b00001000
#define RESIZE_MODE_LEFT 0b00010000

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
        bool showInTaskbar = true;
        bool minimizeButtonVisible = true;
        bool maximizeButtonVisible = true;

        Label label;
        Panel panel;
        MenuBar* menuBar = nullptr;

        enum class Control { None, Minimize, Fullscreen, Close };
        Control pressedControl = Control::None;
        Control hoveredControl = Control::None;
        bool fullscreen = false;
        fenster::Rectangle restoreBounds;

        fenster::Point pressPoint;
        fenster::Point pressScreenPoint;
        fenster::Rectangle pressBounds;
        WindowResizeMode resizeMode;

        int titleHeight;
        int shadowSize;
        fenster::Rectangle crossBounds;
        fenster::Rectangle minimizeBounds;
        fenster::Rectangle fullscreenBounds;

        Control getControlAt(const fenster::Point& position) const;
        fenster::Rectangle getFullscreenBounds();
        void toggleFullscreen();
        void minimize();
        void dragOrResize(MouseEvent& me);
        void startDragOrResize(MouseEvent& me);
        WindowResizeMode getResizeModeAt(const fenster::Point& position) const;

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

        void setMenuBar(MenuBar* menuBar);

        void addChild(Component* component, ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;

        void layout() override;
        void paintBackground(cairo_t* cr) const;
        void paintControls(cairo_t* cr) const;
        void paint() override;

        Component* handleMouseEvent(MouseEvent& e) override;
        std::string getCursorAt(const fenster::Point& position) const override;

        bool isFocusableDefault() const override
        {
            return true;
        }

        void setFocusedInternal(bool focused) override;

        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;
        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        virtual void close();

        void setLayout(Layout* layout) override;
        Layout* getLayout() const override;
    };
}

#endif

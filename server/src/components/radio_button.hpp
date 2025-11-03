// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_RADIOBUTTON
#define FENSTER_SERVER_COMPONENTS_RADIOBUTTON

#include "checkable_component.hpp"
#include "titled_component.hpp"
#include "component.hpp"
#include "label.hpp"


/**
 * Size of the circle
 */
#define FENSTER_RADIOBUTTON_DEFAULT_CIRCLE_SIZE 20

/**
 * Gap between box and text
 */
#define FENSTER_RADIOBUTTON_DEFAULT_TEXT_GAP 5

namespace fensterserver
{
    class RadioButton : virtual public Component, virtual public CheckableComponent, virtual public TitledComponent
    {
        Label label;
        bool checked;
        int boxSize;
        int boxTextGap;

        bool hovered;
        bool pressed;

    public:
        RadioButton();

        void layout() override;
        void paint() override;

        Component* handleMouseEvent(MouseEvent& e) override;
        void handleBoundChanged(const fenster::Rectangle& oldBounds) override;

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        Label& getLabel()
        {
            return label;
        }

        void setCheckedInternal(bool value) override;
    };
}

#endif

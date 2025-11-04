// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_CHECKBOX
#define FENSTER_SERVER_COMPONENTS_CHECKBOX

#include "checkable_component.hpp"
#include "component.hpp"
#include "label.hpp"

/**
 * Size of the rectangular box
 */
#define FENSTER_CHECKBOX_DEFAULT_BOX_SIZE 20

/**
 * Gap between box and text
 */
#define FENSTER_CHECKBOX_DEFAULT_TEXT_GAP 5

namespace fensterserver
{
    class Checkbox : virtual public Component, virtual public TitledComponent, virtual public CheckableComponent
    {
        Label label;
        bool checked;
        int boxSize;
        int boxTextGap;

        bool hovered;
        bool pressed;

    public:
        Checkbox();

        void layout() override;
        void paint() override;

        void setCheckedInternal(bool value) override;
        bool isChecked() override;

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        Component* handleMouseEvent(MouseEvent& e) override;
        void handleBoundChanged(const fenster::Rectangle& oldBounds) override;

        Label& getLabel()
        {
            return label;
        }
    };
}

#endif

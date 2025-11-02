// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_CHECKBOX
#define FENSTER_SERVER_COMPONENTS_CHECKBOX

#include "components/component.hpp"
#include "components/label.hpp"

#define CHECKBOX_DEFAULT_BOX_SIZE 20
#define CHECKBOX_DEFAULT_BOX_TEXT_GAP 5

namespace fensterserver
{
    class Checkbox : virtual public Component
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

        Component* handleMouseEvent(MouseEvent& e) override;
        void handleBoundChanged(const fenster::Rectangle& oldBounds) override;

        Label& getLabel()
        {
            return label;
        }
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_RADIOGROUP
#define FENSTER_SERVER_COMPONENTS_RADIOGROUP

#include "component.hpp"
#include <libfenster/color_argb.hpp>

#include "radio_button.hpp"

namespace fensterserver
{
    class RadioGroup : virtual public Component
    {
        fenster::ColorArgb background;
        void uncheckOthers(RadioButton* selectedRadioButton);

    protected:
        bool hasGraphics() const override
        {
            return background != ARGB(0, 0, 0, 0);
        }

    public:
        RadioGroup();

        void paint() override;

        void setBackground(fenster::ColorArgb color);
        fenster::ColorArgb getBackground();

        void addChild(Component* comp, ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;

        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;
        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;
    };
}

#endif

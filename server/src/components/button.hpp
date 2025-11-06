// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_BUTTON
#define FENSTER_SERVER_COMPONENTS_BUTTON

#include "components/action_component.hpp"
#include "components/button_state.hpp"
#include "components/component.hpp"
#include "components/label.hpp"
#include "components/titled_component.hpp"

#include <libfenster/button.hpp>
#include <libfenster/metrics/insets.hpp>
#include <libfenster/properties.hpp>
#include <string>

namespace fensterserver
{
    class Button : virtual public Component, virtual public TitledComponent, virtual public ActionComponent
    {
        ButtonState state;
        Label label;
        fenster::Insets insets;
        bool enabled;
        fenster::ButtonVariant variant = fenster::Default;

    public:
        Button();
        ~Button() override = default;

        void update() override;
        void layout() override;
        void paint() override;

        Component* handleMouseEvent(MouseEvent& e) override;

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;
        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;

        bool isFocusableDefault() const override;
        void setFocusedInternal(bool focused) override;

        virtual void setEnabled(bool enabled);

        virtual bool isEnabled() const
        {
            return enabled;
        }

        Label& getLabel()
        {
            return label;
        }
    };
}

#endif

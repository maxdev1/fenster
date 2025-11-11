// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_MENUITEM
#define FENSTER_SERVER_COMPONENTS_MENUITEM

#include "component.hpp"
#include "titled_component.hpp"
#include "label.hpp"
#include "action_component.hpp"

#include <libfenster/color_argb.hpp>


namespace fensterserver
{
    class Menu;

    class MenuItem : virtual public Component, virtual public TitledComponent, virtual public ActionComponent
    {
        fenster::ColorArgb background;
        Label label;
        fenster::Insets insets;

        struct
        {
            bool hovered = false;
        } state;

        Menu* subMenu = nullptr;

    protected:
        bool hasGraphics() const override
        {
            return background != ARGB(0, 0, 0, 0);
        }

        bool isFocusableDefault() const override
        {
            return true;
        }

    public:
        MenuItem();

        void paint() override;

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        void layout() override;
        void update() override;

        void setBackground(fenster::ColorArgb color);
        fenster::ColorArgb getBackground();

        void addChild(Component* comp,
                      ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;

        Component* handleMouseEvent(MouseEvent& event) override;

        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;
        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;

        void closeMenu();
    };
}

#endif

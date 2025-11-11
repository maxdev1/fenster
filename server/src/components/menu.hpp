// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_MENU
#define FENSTER_SERVER_COMPONENTS_MENU

#include "component.hpp"
#include <libfenster/color_argb.hpp>

#include "menu_item.hpp"

namespace fensterserver
{
    class Menu : virtual public Component
    {
        fenster::ColorArgb background;

        Menu* openSubMenu = nullptr;

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
        Menu();

        void update() override;
        void paint() override;
        void layout() override;

        void setBackground(fenster::ColorArgb color);
        fenster::ColorArgb getBackground();

        void setFocusedInternal(bool focused) override;

        Component* handleMouseEvent(MouseEvent& event) override;

        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;
        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;
        void close();

        void setOpenSubMenu(Menu* menu)
        {
            openSubMenu = menu;
        }

        Menu* getOpenSubMenu()
        {
            return openSubMenu;
        }
    };
}

#endif

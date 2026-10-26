// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_DESKTOP_SCREEN
#define FENSTER_SERVER_COMPONENTS_DESKTOP_SCREEN

#include <components/menu_item.hpp>

#include "components/component.hpp"

#include <libfenster/metrics/rectangle.hpp>

namespace fensterserver
{
    class Screen : virtual public Component
    {
        fenster::Rectangle invalid;
        Component* desktopCanvas = nullptr;
        Component* taskbar = nullptr;
        Component* desktopPopup = nullptr;
        fenster::ComponentId popupPreviousFocus = -1;

        bool pressing = false;
        fenster::Point pressPoint;

    public:
        ~Screen() override = default;

        void setDesktopCanvas(Component* canvas) { desktopCanvas = canvas; }
        bool setTaskbar(Component* component, bool enabled);
        bool isTaskbar(const Component* component) const { return taskbar == component; }
        bool setDesktopPopup(Component* component, bool visible);
        void dismissDesktopPopup(bool restoreFocus);
        Component* getDesktopPopup() const { return desktopPopup && desktopPopup->isVisible() ? desktopPopup : nullptr; }
        Component* handleMouseEvent(MouseEvent& event) override;
        fenster::Rectangle getWindowArea();
        void layout() override;

        void addChild(Component* comp,
                      ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT) override;
        void removeChild(Component* comp) override;

        void sendWindowEvent(fenster::ComponentId observerId, Window* window, SYS_TID_T observerThread, bool present);

        /**
         * Overrides the default invalidation method. On the component, this method
         * just dispatches to the parent, but here we must remember the invalidation.
         */
        virtual void markDirty(fenster::Rectangle rect);

        fenster::Rectangle grabInvalid();

        /**
         * Shows a menu on screen.
         */
        void showMenu(Menu* menu, fenster::Point position);

        /**
         * Hides all open menus.
         */
        void hideMenus();
    };
}

#endif

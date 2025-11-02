// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_SCROLLPANE
#define FENSTER_SERVER_COMPONENTS_SCROLLPANE

#include "components/component.hpp"
#include "components/scrollbar.hpp"

namespace fensterserver
{
    class ScrollPane : virtual public Component, public ScrollHandler
    {
        Component* content = nullptr;
        fenster::Point scrollPosition = fenster::Point(0, 0);
        Scrollbar verticalScrollbar = Scrollbar(ScrollbarOrientation::VERTICAL);
        Scrollbar horizontalScrollbar = Scrollbar(ScrollbarOrientation::HORIZONTAL);

        bool fixedWidth = false;
        bool fixedHeight = false;

        bool showHbar = false;
        bool showVbar = false;

        virtual void updateContent();

    protected:
        bool hasGraphics() const override
        {
            return false;
        }

    public:
        ScrollPane();

        virtual fenster::Point getPosition() const
        {
            return scrollPosition;
        }

        void layout() override;

        Component* handleMouseEvent(MouseEvent& event) override;
        void handleScroll(Scrollbar* bar) override;

        virtual void setContent(Component* content);

        virtual Component* getContent() const
        {
            return content;
        }

        fenster::Dimension calculateViewport(fenster::Dimension contentPrefSize);

        void setFixedWidth(bool fix)
        {
            this->fixedWidth = fix;
        }

        void setFixedHeight(bool fix)
        {
            this->fixedHeight = fix;
        }
    };
}

#endif

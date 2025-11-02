// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_SCROLLBAR
#define FENSTER_SERVER_COMPONENTS_SCROLLBAR

#include "components/button_state.hpp"
#include "components/component.hpp"

namespace fensterserver
{
    class Scrollbar;

    enum class ScrollbarOrientation : uint8_t
    {
        VERTICAL,
        HORIZONTAL
    };

    class ScrollHandler
    {
    public:
        virtual ~ScrollHandler() = default;
        virtual void handleScroll(Scrollbar* bar) = 0;
    };

    class Scrollbar : public Component
    {
        ScrollbarOrientation orientation;

        int modelPosition;
        int viewportLength;
        int contentLength;

        ButtonState decButtonState;
        ButtonState incButtonState;

        bool knobDrag;
        int dragPressPosition;
        int dragViewPosition;

        ScrollHandler* scrollHandler;

    public:
        explicit Scrollbar(ScrollbarOrientation orientation) : orientation(orientation), modelPosition(0),
                                                               viewportLength(0), contentLength(0), knobDrag(false),
                                                               dragPressPosition(0), dragViewPosition(0),
                                                               scrollHandler(0)
        {
        }

        void paint() override;

        Component* handleMouseEvent(MouseEvent& e) override;

        virtual void setScrollHandler(ScrollHandler* handler)
        {
            scrollHandler = handler;
        }

        virtual ScrollHandler* getScrollHandler()
        {
            return scrollHandler;
        }

        void setViewLengths(int visibleArea, int totalArea);

        int getViewportLength() const
        {
            return viewportLength;
        }

        int getContentLength() const
        {
            return contentLength;
        }

        void setModelPosition(int position);

        int getModelPosition() const
        {
            return modelPosition;
        }

        int getKnobSpace();
        int getKnobLength();

        fenster::Rectangle calculateKnob();
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_SCROLLBAR__
#define __WINDOWSERVER_COMPONENTS_SCROLLBAR__

#include "components/button_state.hpp"
#include "components/component.hpp"

class scrollbar_t;

enum class scrollbar_orientation_t : uint8_t
{
    VERTICAL,
    HORIZONTAL
};

class scroll_handler_t
{
public:
    virtual ~scroll_handler_t() = default;
    virtual void handleScroll(scrollbar_t* bar) = 0;
};

class scrollbar_t : public component_t
{
    scrollbar_orientation_t orientation;

    int modelPosition;
    int viewportLength;
    int contentLength;

    button_state_t decButtonState;
    button_state_t incButtonState;

    bool knobDrag;
    int dragPressPosition;
    int dragViewPosition;

    scroll_handler_t* scrollHandler;

public:
    explicit scrollbar_t(scrollbar_orientation_t orientation) : orientation(orientation), modelPosition(0),
                                                                viewportLength(0), contentLength(0), knobDrag(false),
                                                                dragPressPosition(0), dragViewPosition(0),
                                                                scrollHandler(0)
    {
    }

    void paint() override;

    component_t* handleMouseEvent(mouse_event_t& e) override;

    virtual void setScrollHandler(scroll_handler_t* handler)
    {
        scrollHandler = handler;
    }

    virtual scroll_handler_t* getScrollHandler()
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

    g_rectangle calculateKnob();
};

#endif

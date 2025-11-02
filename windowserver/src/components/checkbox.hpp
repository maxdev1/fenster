// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_CHECKBOX__
#define __WINDOWSERVER_COMPONENTS_CHECKBOX__

#include "components/component.hpp"
#include "components/label.hpp"

#define DEFAULT_BOX_SIZE 20
#define DEFAULT_BOX_TEXT_GAP 5

class checkbox_t : virtual public component_t
{
    label_t label;
    bool checked;
    int boxSize;
    int boxTextGap;

    bool hovered;
    bool pressed;

public:
    checkbox_t();

    void layout() override;
    void paint() override;

    component_t* handleMouseEvent(mouse_event_t& e) override;
    void handleBoundChanged(const g_rectangle& oldBounds) override;

    label_t& getLabel()
    {
        return label;
    }
};

#endif

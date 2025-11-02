// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_TEXT_TEXTCOMPONENT__
#define __WINDOWSERVER_COMPONENTS_TEXT_TEXTCOMPONENT__

#include "components/component.hpp"
#include "components/text/move/caret_move_strategy.hpp"
#include <libwindow/metrics/range.hpp>

class text_component_t : virtual public component_t
{
protected:
    caret_move_strategy_t* caretMoveStrategy = nullptr;

public:
    ~text_component_t() override = default;

    virtual void setCursor(int position) = 0;
    virtual int getCursor() = 0;

    virtual void setMarker(int position) = 0;
    virtual int getMarker() = 0;

    virtual void setText(std::string text) = 0;
    virtual std::string getText() = 0;

    virtual g_range getSelectedRange() = 0;
};

#endif

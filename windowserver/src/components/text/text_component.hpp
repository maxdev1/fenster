// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TEXTCOMPONENT
#define FENSTER_SERVER_COMPONENTS_TEXTCOMPONENT

#include "components/component.hpp"
#include "components/text/move/caret_move_strategy.hpp"
#include <libwindow/metrics/range.hpp>

namespace fensterserver
{
    class TextComponent : virtual public Component
    {
    protected:
        CaretMoveStrategy* caretMoveStrategy = nullptr;

    public:
        ~TextComponent() override = default;

        virtual void setCursor(int position) = 0;
        virtual int getCursor() = 0;

        virtual void setMarker(int position) = 0;
        virtual int getMarker() = 0;

        virtual void setText(std::string text) = 0;
        virtual std::string getText() = 0;

        virtual fenster::Range getSelectedRange() = 0;
    };
}

#endif

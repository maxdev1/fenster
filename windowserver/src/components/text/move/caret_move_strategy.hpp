// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TEXT_MOVE_CARETMOVESTRATEGY
#define FENSTER_SERVER_COMPONENTS_TEXT_MOVE_CARETMOVESTRATEGY

#include "components/text/caret_direction.hpp"
#include <libwindow/input/key_info.hpp>

namespace fensterserver
{
    class TextComponent;

    class CaretMoveStrategy
    {
    public:
        virtual ~CaretMoveStrategy()
        {
        }

        virtual void moveCaret(TextComponent* component, CaretDirection direction, fenster::KeyInfo& keyInfo) = 0;

        virtual int calculateSkip(std::string text, int position, CaretDirection direction) = 0;
    };
}

#endif

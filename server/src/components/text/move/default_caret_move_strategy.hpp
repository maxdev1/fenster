// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TEXT_MOVE_DEFAULTCARETMOVESTRATEGY
#define FENSTER_SERVER_COMPONENTS_TEXT_MOVE_DEFAULTCARETMOVESTRATEGY

#include "components/text/move/caret_move_strategy.hpp"
#include <libfenster/input/key_info.hpp>

namespace fensterserver
{
    class DefaultCaretMoveStrategy : public CaretMoveStrategy
    {
        DefaultCaretMoveStrategy()
        {
        }

    public:
        virtual ~DefaultCaretMoveStrategy()
        {
        }

        virtual void moveCaret(TextComponent* component, CaretDirection direction, fenster::KeyInfo& keyInfo);

        virtual int calculateSkip(std::string text, int position, CaretDirection direction);

        static DefaultCaretMoveStrategy* getInstance();
    };
}

#endif

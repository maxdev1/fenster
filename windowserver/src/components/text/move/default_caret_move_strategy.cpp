// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/text/move/default_caret_move_strategy.hpp"
#include "components/text/text_component.hpp"

static default_caret_move_strategy_t* INSTANCE = 0;

default_caret_move_strategy_t* default_caret_move_strategy_t::getInstance()
{
    if(INSTANCE == 0)
    {
        INSTANCE = new default_caret_move_strategy_t();
    }
    return INSTANCE;
}

void default_caret_move_strategy_t::moveCaret(text_component_t* component, caret_direction_t direction, key_info_t& info)
{

    int cursor = component->getCursor();
    int newCursorPosition = cursor;
    int selectedLength = component->getSelectedRange().getLength();

    if(direction == caret_direction_t::RIGHT)
    {
        if(info.ctrl)
        {
            newCursorPosition = component->getText().length();
        }
        else
        {
            newCursorPosition = info.alt ? calculateSkip(component->getText(), cursor, caret_direction_t::RIGHT) : (cursor + 1);
        }
    }
    else if(direction == caret_direction_t::LEFT)
    {
        if(info.ctrl)
        {
            newCursorPosition = 0;
        }
        else
        {
            newCursorPosition = info.alt ? calculateSkip(component->getText(), cursor, caret_direction_t::LEFT) : (cursor - 1);
        }
    }

    component->setCursor(newCursorPosition);
    if(!info.shift)
    {
        component->setMarker(newCursorPosition);
    }
}

int default_caret_move_strategy_t::calculateSkip(std::string text, int position, caret_direction_t direction)
{

    bool l = (direction == caret_direction_t::LEFT);

    if(l ? (position > 0) : (position < text.length()))
    {
        bool inFirst = true;
        if(!l)
        {
            char c = text[position];
            if(!(c == ' ' || c == ',' || c == '.'))
            {
                inFirst = false;
            }
        }

        l ? --position : ++position;

        while(l ? position > 0 : position < text.length())
        {
            char c = text[position];
            char p = text[l ? position - 1 : position];

            if(inFirst && (c == ' ' || c == ',' || c == '.'))
            {
                l ? --position : ++position;
            }
            else if((!l || !(p == ' ' || p == ',' || p == '.')) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
            {
                l ? --position : ++position;
                inFirst = false;
            }
            else
            {
                break;
            }
        }
    }

    return position;
}

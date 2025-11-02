// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_TEXT_MOVE_DEFAULTCARETMOVESTRATEGY__
#define __WINDOWSERVER_COMPONENTS_TEXT_MOVE_DEFAULTCARETMOVESTRATEGY__

#include "components/text/move/caret_move_strategy.hpp"
#include <libwindow/input/key_info.hpp>

class default_caret_move_strategy_t : public caret_move_strategy_t
{
  private:
    default_caret_move_strategy_t()
    {
    }

  public:
    virtual ~default_caret_move_strategy_t()
    {
    }

    virtual void moveCaret(text_component_t* component, caret_direction_t direction, key_info_t& keyInfo);

    virtual int calculateSkip(std::string text, int position, caret_direction_t direction);

    static default_caret_move_strategy_t* getInstance();
};

#endif

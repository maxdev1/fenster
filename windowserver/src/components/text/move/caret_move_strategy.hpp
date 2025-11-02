// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_TEXT_MOVE_CARETMOVESTRATEGY__
#define __WINDOWSERVER_COMPONENTS_TEXT_MOVE_CARETMOVESTRATEGY__

#include "components/text/caret_direction.hpp"
#include <libwindow/platform/platform-key-info.hpp>

class text_component_t;

class caret_move_strategy_t
{
  public:
    virtual ~caret_move_strategy_t()
    {
    }

    virtual void moveCaret(text_component_t* component, caret_direction_t direction, g_key_info& keyInfo) = 0;

    virtual int calculateSkip(std::string text, int position, caret_direction_t direction) = 0;
};

#endif

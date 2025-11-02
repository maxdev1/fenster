// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __STACK_LAYOUT_MANAGER__
#define __STACK_LAYOUT_MANAGER__

#include "layout_manager.hpp"

/**
 *
 */
class stack_layout_manager_t : public layout_manager_t
{
    bool horizontal = false;

public:
    virtual void layout();

    void setHorizontal(bool horizontal)
    {
        this->horizontal = horizontal;
    }
};

#endif

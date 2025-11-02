// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __FLOW_LAYOUT_MANAGER__
#define __FLOW_LAYOUT_MANAGER__

#include "layout_manager.hpp"

/**
 *
 */
class flow_layout_manager_t : public layout_manager_t
{
    g_insets padding;

public:
    virtual void layout();

    void setPadding(g_insets padding) override
    {
        this->padding = padding;
    }
};

#endif

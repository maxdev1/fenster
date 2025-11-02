// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __FLEX_LAYOUT_MANAGER__
#define __FLEX_LAYOUT_MANAGER__

#include "layout_manager.hpp"
#include <unordered_map>
#include <libwindow/metrics/insets.hpp>

struct flex_info_t
{
    float grow;
    float shrink;
    int basis;
};

class flex_layout_manager_t : public layout_manager_t
{
    std::unordered_map<component_t*, flex_info_t> flexInfo;
    bool horizontal = true;
    g_insets padding = g_insets(0, 0, 0, 0);
    int gap = 0;

public:
    void layout() override;

    void setLayoutInfo(component_t* child, float grow, float shrink, int basis);

    void setHorizontal(bool horizontal)
    {
        this->horizontal = horizontal;
    }

    int getGap()
    {
        return gap;
    }

    void setGap(int gap)
    {
        this->gap = gap;
    }

    void setPadding(g_insets padding) override
    {
        this->padding = padding;
    }
};

#endif

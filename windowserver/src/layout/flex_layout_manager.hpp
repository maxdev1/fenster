// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FLEXLAYOUTMANAGER
#define FENSTER_SERVER_LAYOUT_FLEXLAYOUTMANAGER

#include "layout_manager.hpp"
#include <unordered_map>
#include <libwindow/metrics/insets.hpp>

namespace fensterserver
{
    struct FlexInfo
    {
        float grow;
        float shrink;
        int basis;
    };

    class FlexLayoutManager : public LayoutManager
    {
        std::unordered_map<Component*, FlexInfo> flexInfo;
        bool horizontal = true;
        fenster::Insets padding = fenster::Insets(0, 0, 0, 0);
        int gap = 0;

    public:
        void layout() override;

        void setLayoutInfo(Component* child, float grow, float shrink, int basis);

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

        void setPadding(fenster::Insets padding) override
        {
            this->padding = padding;
        }
    };
}

#endif

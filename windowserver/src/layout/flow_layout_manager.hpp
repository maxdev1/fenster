// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FLOWLAYOUTMANAGER
#define FENSTER_SERVER_LAYOUT_FLOWLAYOUTMANAGER

#include "layout_manager.hpp"

namespace fensterserver
{
    class FlowLayoutManager : public LayoutManager
    {
        fenster::Insets padding;

    public:
        virtual void layout();

        void setPadding(fenster::Insets padding) override
        {
            this->padding = padding;
        }
    };
}

#endif

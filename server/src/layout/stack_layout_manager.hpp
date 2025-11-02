// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_STACKLAYOUTMANAGER
#define FENSTER_SERVER_LAYOUT_STACKLAYOUTMANAGER

#include "layout_manager.hpp"

namespace fensterserver
{
    /**
     *
     */
    class StackLayoutManager : public LayoutManager
    {
        bool horizontal = false;

    public:
        virtual void layout();

        void setHorizontal(bool horizontal)
        {
            this->horizontal = horizontal;
        }
    };
}

#endif

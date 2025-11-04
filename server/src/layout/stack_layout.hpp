// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_STACKLAYOUT
#define FENSTER_SERVER_LAYOUT_STACKLAYOUT

#include "layout.hpp"

namespace fensterserver
{
    class StackLayout : public Layout
    {
        bool horizontal = false;
        fenster::Insets padding;
        int space = 0;

    public:
        StackLayout() = default;
        explicit StackLayout(int space) : space(space)
        {
        }

        void layout() override;

        void setPadding(fenster::Insets insets)
        {
            padding = insets;
        }

        void setSpace(int space)
        {
            this->space = space;
        }

        void setHorizontal(bool horizontal)
        {
            this->horizontal = horizontal;
        }
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FLOWLAYOUT
#define FENSTER_SERVER_LAYOUT_FLOWLAYOUT

#include "layout.hpp"

namespace fensterserver
{
    class FlowLayout : public Layout
    {
        fenster::Insets padding;

    public:
        void layout() override;

        void setPadding(fenster::Insets padding)
        {
            this->padding = padding;
        }
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FILLLAYOUT
#define FENSTER_SERVER_LAYOUT_FILLLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"

namespace fensterserver
{
    class FillLayout : public Layout, public PaddingSupport
    {
    public:
        explicit FillLayout()
            :  PaddingSupport(fenster::Insets(0, 0, 0, 0))
        {
        }

        void layout() override;
    };
}

#endif

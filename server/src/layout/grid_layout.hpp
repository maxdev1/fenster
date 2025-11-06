// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_GRIDLAYOUT
#define FENSTER_SERVER_LAYOUT_GRIDLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/spacing_support.hpp"

namespace fensterserver
{
    class GridLayout : public Layout, public PaddingSupport, public SpacingSupport
    {
        int columns = 1;
        int rows = 0;

    public:
        explicit GridLayout(int columns = 1, int rows = 0, int horizontalSpace = 0, int verticalSpace = 0)
            : columns(columns), rows(rows), PaddingSupport(fenster::Insets(0, 0, 0, 0)),
              SpacingSupport(horizontalSpace, verticalSpace)
        {
        }

        void layout() override;
    };
}

#endif

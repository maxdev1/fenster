// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_GRIDLAYOUT
#define FENSTER_SERVER_LAYOUT_GRIDLAYOUT

#include <libfenster/metrics/insets.hpp>
#include "layout.hpp"

namespace fensterserver
{
    class GridLayout : public Layout
    {
        int columns;
        int rows;
        fenster::Insets padding;
        int rowSpace;
        int colSpace;

    public:
        GridLayout(int columns = 1, int rows = 0, int rowSpace = 0, int columnSpace = 0)
            : columns(columns), rows(rows), padding(fenster::Insets(0, 0, 0, 0)),
              rowSpace(rowSpace), colSpace(columnSpace)
        {
        }

        void setPadding(fenster::Insets padding)
        {
            this->padding = padding;
        }

        fenster::Insets getPadding() const
        {
            return padding;
        }

        void setRowSpace(int _space)
        {
            rowSpace = _space;
        }

        int getRowSpace() const
        {
            return rowSpace;
        }

        void setColSpace(int _space)
        {
            colSpace = _space;
        }

        int getColSpace() const
        {
            return colSpace;
        }

        virtual void layout();
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_GRIDLAYOUTMANAGER
#define FENSTER_SERVER_LAYOUT_GRIDLAYOUTMANAGER

#include "layout_manager.hpp"
#include <libfenster/metrics/insets.hpp>

namespace fensterserver
{
    class GridLayoutManager : public LayoutManager
    {
        int columns;
        int rows;
        fenster::Insets padding;
        int rowSpace;
        int colSpace;

    public:
        GridLayoutManager(int columns = 1, int rows = 0, int rowSpace = 0, int columnSpace = 0)
            : columns(columns), rows(rows), padding(fenster::Insets(0, 0, 0, 0)),
              rowSpace(rowSpace), colSpace(columnSpace)
        {
        }

        void setPadding(fenster::Insets _padding) override
        {
            padding = _padding;
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

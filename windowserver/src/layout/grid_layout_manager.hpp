// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_LAYOUT_GRIDLAYOUTMANAGER__
#define __WINDOWSERVER_LAYOUT_GRIDLAYOUTMANAGER__

#include "layout_manager.hpp"
#include <libwindow/metrics/insets.hpp>

class grid_layout_manager_t : public layout_manager_t
{
    int columns;
    int rows;
    g_insets padding;
    int rowSpace;
    int colSpace;

public:
    grid_layout_manager_t(int columns = 1, int rows = 0, int rowSpace = 0, int columnSpace = 0)
        : columns(columns), rows(rows), padding(g_insets(0, 0, 0, 0)),
          rowSpace(rowSpace), colSpace(columnSpace)
    {
    }

    void setPadding(g_insets _padding) override
    {
        padding = _padding;
    }

    g_insets getPadding() const
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

#endif

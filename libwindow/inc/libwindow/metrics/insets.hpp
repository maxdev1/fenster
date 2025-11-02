// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_METRICS_INSETS__
#define __LIBWINDOW_METRICS_INSETS__

#include "point.hpp"
#include <cstdint>

class g_insets
{
public:
    int top;
    int left;
    int bottom;
    int right;

    g_insets() : top(0), left(0), bottom(0), right(0)
    {
    }

    g_insets(int top, int left, int bottom, int right) : top(top), left(left), bottom(bottom), right(right)
    {
    }

    g_insets& operator=(const g_insets& rhs)
    {
        top = rhs.top;
        left = rhs.left;
        bottom = rhs.bottom;
        right = rhs.right;
        return *this;
    }

    bool operator==(const g_insets& rhs) const
    {
        return top == rhs.top && left == rhs.left && bottom == rhs.bottom && right == rhs.right;
    }

    bool operator!=(const g_insets& rhs) const
    {
        return !(*this == rhs);
    }
}__attribute__((packed));;

#endif

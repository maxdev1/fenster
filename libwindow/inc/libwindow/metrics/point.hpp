// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_METRICS_POINT__
#define __LIBWINDOW_METRICS_POINT__

#include <cstdint>

class g_point
{
  public:
    int32_t x;
    int32_t y;

    g_point() : x(0), y(0)
    {
    }

    g_point(int32_t _x, int32_t _y) : x(_x), y(_y)
    {
    }

    g_point(const g_point& p) : x(p.x), y(p.y)
    {
    }

    g_point& operator=(const g_point& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    g_point& operator+=(const g_point& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    bool operator==(const g_point& p) const
    {
        return x == p.x && y == p.y;
    }

    bool operator!=(const g_point& p) const
    {
        return !(*this == p);
    }

    g_point operator-(const g_point& p) const
    {
        return g_point(x - p.x, y - p.y);
    }

    g_point operator+(const g_point& p) const
    {
        return g_point(x + p.x, y + p.y);
    }

} __attribute__((packed));

#endif

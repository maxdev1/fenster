// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_METRICS_RANGE__
#define __LIBWINDOW_METRICS_RANGE__

#include <cstdint>

class g_range
{
  private:
    int32_t first;
    int32_t last;

  public:
    g_range() : first(0), last(0)
    {
    }

    g_range(int32_t _a, int32_t _b)
    {
        set(_a, _b);
    }

    g_range(const g_range& p) : first(p.first), last(p.last)
    {
    }

    void set(int32_t a, int32_t b)
    {
        first = a < b ? a : b;
        last = a > b ? a : b;
    }

    int32_t getFirst()
    {
        return first;
    }

    int32_t getLast()
    {
        return last;
    }

    int32_t getLength()
    {
        return last - first;
    }

    g_range& operator=(const g_range& rhs)
    {
        first = rhs.first;
        last = rhs.last;
        return *this;
    }

    bool operator==(const g_range& p) const
    {
        return (first == p.first) && (last == p.last);
    }

    bool operator!=(const g_range& p) const
    {
        return !(*this == p);
    }
};

#endif

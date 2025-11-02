// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_METRICS_POINT__
#define __LIBWINDOW_METRICS_POINT__

#include <cstdint>

namespace fenster
{
	class Point
	{
	public:
		int32_t x;
		int32_t y;

		Point() : x(0), y(0)
		{
		}

		Point(int32_t _x, int32_t _y) : x(_x), y(_y)
		{
		}

		Point(const Point& p) : x(p.x), y(p.y)
		{
		}

		Point& operator=(const Point& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			return *this;
		}

		Point& operator+=(const Point& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		bool operator==(const Point& p) const
		{
			return x == p.x && y == p.y;
		}

		bool operator!=(const Point& p) const
		{
			return !(*this == p);
		}

		Point operator-(const Point& p) const
		{
			return Point(x - p.x, y - p.y);
		}

		Point operator+(const Point& p) const
		{
			return Point(x + p.x, y + p.y);
		}

	} __attribute__((packed));
}

#endif

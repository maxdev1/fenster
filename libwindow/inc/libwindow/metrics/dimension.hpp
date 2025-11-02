// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_METRICS_DIMENSION__
#define __LIBWINDOW_METRICS_DIMENSION__

#include "point.hpp"
#include <cstdint>

namespace fenster
{
	class Dimension
	{
	public:
		int32_t width;
		int32_t height;

		Dimension() :
			width(0), height(0)
		{
		}

		Dimension(int32_t width, int32_t height) :
			width(width), height(height)
		{
		}

		Dimension(const Dimension& p) :
			width(p.width), height(p.height)
		{
		}

		Dimension& operator=(const Dimension& rhs)
		{
			width = rhs.width;
			height = rhs.height;
			return *this;
		}

		bool operator==(const Dimension& p) const
		{
			return width == p.width && height == p.height;
		}

		bool operator!=(const Dimension& p) const
		{
			return !(*this == p);
		}

		Point operator-(const Dimension& p) const
		{
			return Point(width - p.width, height - p.height);
		}

		Point operator+(const Dimension& p) const
		{
			return Point(width + p.width, height + p.height);
		}

	} __attribute__((packed));
}

#endif

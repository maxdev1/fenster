// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_METRICS_DIMENSION__
#define __LIBWINDOW_METRICS_DIMENSION__

#include "point.hpp"
#include <cstdint>

class g_dimension
{
  public:
	int32_t width;
	int32_t height;

	g_dimension() : width(0), height(0)
	{
	}

	g_dimension(int32_t width, int32_t height) : width(width), height(height)
	{
	}

	g_dimension(const g_dimension& p) : width(p.width), height(p.height)
	{
	}

	g_dimension& operator=(const g_dimension& rhs)
	{
		width = rhs.width;
		height = rhs.height;
		return *this;
	}

	bool operator==(const g_dimension& p) const
	{
		return width == p.width && height == p.height;
	}

	bool operator!=(const g_dimension& p) const
	{
		return !(*this == p);
	}

	g_point operator-(const g_dimension& p) const
	{
		return g_point(width - p.width, height - p.height);
	}

	g_point operator+(const g_dimension& p) const
	{
		return g_point(width + p.width, height + p.height);
	}

} __attribute__((packed));

#endif

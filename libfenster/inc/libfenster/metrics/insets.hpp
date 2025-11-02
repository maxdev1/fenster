// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_METRICS_INSETS
#define LIBFENSTER_METRICS_INSETS

#include "point.hpp"
#include <cstdint>

namespace fenster
{
	class Insets
	{
	public:
		int top;
		int left;
		int bottom;
		int right;

		Insets() :
			top(0), left(0), bottom(0), right(0)
		{
		}

		Insets(int top, int left, int bottom, int right) :
			top(top), left(left), bottom(bottom), right(right)
		{
		}

		Insets& operator=(const Insets& rhs)
		{
			top = rhs.top;
			left = rhs.left;
			bottom = rhs.bottom;
			right = rhs.right;
			return *this;
		}

		bool operator==(const Insets& rhs) const
		{
			return top == rhs.top && left == rhs.left && bottom == rhs.bottom && right == rhs.right;
		}

		bool operator!=(const Insets& rhs) const
		{
			return !(*this == rhs);
		}
	}__attribute__((packed));;
}

#endif

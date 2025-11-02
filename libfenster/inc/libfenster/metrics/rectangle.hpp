// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_METRICS_RECTANGLE
#define LIBFENSTER_METRICS_RECTANGLE

#include "dimension.hpp"
#include "insets.hpp"
#include "point.hpp"
#include <cstdint>
#include <algorithm>

namespace fenster
{
	struct Rectangle
	{
	public:
		int x;
		int y;
		int width;
		int height;

		Rectangle() :
			x(0), y(0), width(0), height(0)
		{
		}

		Rectangle(int x, int y, int width, int height) :
			x(x), y(y), width(width), height(height)
		{
		}

		Rectangle(const Rectangle& rhs) :
			x(rhs.x), y(rhs.y), width(rhs.width), height(rhs.height)
		{
		}

		Rectangle& operator=(const Rectangle& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			width = rhs.width;
			height = rhs.height;
			return *this;
		}

		bool operator==(const Rectangle& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		bool operator!=(const Rectangle& rhs) const
		{
			return !(*this == rhs);
		}

		Rectangle& operator-=(const Insets& rhs)
		{
			x = x + rhs.left;
			y = y + rhs.top;
			width = width - rhs.left - rhs.right;
			height = height - rhs.top - rhs.bottom;
			return *this;
		}

		bool contains(Point p) const
		{
			return (p.x >= x) && (p.y >= y) && (p.x < x + width) && (p.y < y + height);
		}

		int getTop() const
		{
			return y;
		}

		void setTop(int _top)
		{
			y = _top;
		}

		int getLeft() const
		{
			return x;
		}

		void setLeft(int _left)
		{
			x = _left;
		}

		int getBottom() const
		{
			return y + height;
		}

		void setBottom(int32_t _bottom)
		{
			height = _bottom - y;
		}

		int getRight() const
		{
			return x + width;
		}

		void setRight(int _right)
		{
			width = _right - x;
		}

		void setStart(Point p)
		{
			x = p.x;
			y = p.y;
		}

		Point getStart() const
		{
			return Point(x, y);
		}

		void setEnd(Point p)
		{
			width = p.x - x;
			height = p.y - y;
		}

		Point getEnd() const
		{
			return Point(x + width, y + height);
		}

		Rectangle asNormalized()
		{
			Rectangle norm = *this;

			if (norm.width < 0)
			{
				norm.x += norm.width;
				norm.width = -norm.width;
			}
			if (norm.height < 0)
			{
				norm.y += norm.height;
				norm.height = -norm.height;
			}
			return norm;
		}

		Dimension getSize()
		{
			return Dimension(width, height);
		}

		bool intersects(Rectangle p) const
		{
			return !(x + width <= p.x || p.x + p.width <= x || y + height <= p.y || p.y + p.height <= y);
		}

		/**
		 * Extends this rectangle so the point is included.
		 */
		void extend(const Point& p)
		{
			if (width <= 0 || height <= 0)
			{
				x = p.x;
				y = p.y;
				width = 1;
				height = 1;
				return;
			}

			int right = std::max(x + width, p.x + 1);
			int bottom = std::max(y + height, p.y + 1);
			x = std::min(x, p.x);
			y = std::min(y, p.y);
			width = right - x;
			height = bottom - y;
		}

		/**
		 * Clips this rectangle to be within the given rectangle.
		 */
		[[nodiscard]] Rectangle clip(const Rectangle& other) const
		{
			// Find the intersecting rectangle coordinates
			int left = std::max(x, other.x);
			int top = std::max(y, other.y);
			int right = std::min(x + width, other.x + other.width);
			int bottom = std::min(y + height, other.y + other.height);

			// If there's no intersection, return a rectangle with zero dimensions
			if (left >= right || top >= bottom)
			{
				return {0, 0, 0, 0};
			}

			// Return the intersecting rectangle
			return {
				left,
				top,
				right - left,
				bottom - top
			};
		}
	} __attribute__((packed));
}

#endif

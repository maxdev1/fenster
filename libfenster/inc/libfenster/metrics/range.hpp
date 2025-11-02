// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_METRICS_RANGE
#define LIBFENSTER_METRICS_RANGE

#include <cstdint>

namespace fenster
{
	class Range
	{
		int32_t first;
		int32_t last;

	public:
		Range() : first(0), last(0)
		{
		}

		Range(int32_t _a, int32_t _b)
		{
			set(_a, _b);
		}

		Range(const Range& p) : first(p.first), last(p.last)
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

		Range& operator=(const Range& rhs)
		{
			first = rhs.first;
			last = rhs.last;
			return *this;
		}

		bool operator==(const Range& p) const
		{
			return (first == p.first) && (last == p.last);
		}

		bool operator!=(const Range& p) const
		{
			return !(*this == p);
		}
	};
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_LISTENER
#define LIBWINDOW_LISTENER

#include "libwindow/interface.hpp"

namespace fenster
{
	class Listener
	{
	public:
		virtual ~Listener() = default;

		virtual void process(ComponentEventHeader* header) = 0;
	};
}

#endif

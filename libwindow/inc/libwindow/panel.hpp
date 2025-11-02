// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_PANEL
#define LIBWINDOW_PANEL

#include "component.hpp"

namespace fenster
{
	class Panel : virtual public Component
	{
	public:
		explicit Panel(ComponentId id) : Component(id)
		{
		}

		static Panel* create();
	};
}

#endif

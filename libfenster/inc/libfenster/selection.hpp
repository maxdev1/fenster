// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_SELECTION
#define LIBFENSTER_SELECTION

#include "component.hpp"

namespace fenster
{
	class Selection : virtual public Component
	{
	public:
		explicit Selection(ComponentId id) :
			Component(id)
		{
		}

		static Selection* create();
	};
}

#endif

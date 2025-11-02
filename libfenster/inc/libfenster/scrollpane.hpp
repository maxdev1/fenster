// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_SCROLLPANE
#define LIBFENSTER_SCROLLPANE

#include "component.hpp"

namespace fenster
{
	class Scrollpane : virtual public Component
	{
	public:
		explicit Scrollpane(ComponentId id) :
			Component(id)
		{
		}

		static Scrollpane* create();

		bool setContent(Component* content);
		bool setFixed(bool width, bool height);
	};
}

#endif

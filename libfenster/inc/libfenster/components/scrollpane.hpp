// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_SCROLLPANE
#define LIBFENSTER_SCROLLPANE

#include "component.hpp"

namespace fenster
{
	class ScrollPane : virtual public Component
	{
	public:
		explicit ScrollPane(ComponentId id) :
			Component(id)
		{
		}

		static ScrollPane* create();

		bool setContent(Component* content);
		bool setFixed(bool width, bool height);
	};
}

#endif

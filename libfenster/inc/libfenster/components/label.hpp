// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LABEL
#define LIBFENSTER_LABEL

#include "component.hpp"
#include "titled_component.hpp"
#include "libfenster/font/text_alignment.hpp"

namespace fenster
{
	class Label : virtual public Component, virtual public TitledComponent
	{
	public:
		explicit Label(ComponentId id) :
			Component(id), TitledComponent(id)
		{
		}

		static Label* create();

		bool setColor(ColorArgb argb);
		bool setAlignment(TextAlignment alignment);
	};
}

#endif

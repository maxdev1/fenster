// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_TEXTAREA
#define LIBFENSTER_TEXTAREA

#include "component.hpp"
#include "titled_component.hpp"

namespace fenster
{
	class TextArea : virtual public Component, virtual public TitledComponent
	{
	public:
		explicit TextArea(ComponentId id) :
			Component(id), TitledComponent(id)
		{
		}

		static TextArea* create();
	};
}

#endif

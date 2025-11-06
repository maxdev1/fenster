// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/stack_layout.hpp"
#include "libfenster/component.hpp"

namespace fenster
{
	StackLayout* StackLayout::create(Component* component)
	{
		auto layout = new StackLayout(component);
		component->setLayout(layout);
		return layout;
	}


}

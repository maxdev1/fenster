// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/grid_layout.hpp"
#include "libfenster/component.hpp"

namespace fenster
{
	GridLayout* GridLayout::create(Component* component)
	{
		auto layout = new GridLayout(component);
		component->setLayout(layout);
		return layout;
	}
}

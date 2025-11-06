// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/flow_layout.hpp"
#include "libfenster/component.hpp"

namespace fenster
{
	FlowLayout* FlowLayout::create(Component* component)
	{
		auto layout = new FlowLayout(component);
		component->setLayout(layout);
		return layout;
	}
}

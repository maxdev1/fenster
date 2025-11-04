// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/grid_layout_manager.hpp"
#include "libfenster/component.hpp"
#include "libfenster/json/json.hpp"
#include "libfenster/properties.hpp"
#include <sstream>
#include <map>

namespace fenster
{
	GridLayoutManager* GridLayoutManager::create(Component* component)
	{
		auto layout = new GridLayoutManager(component);
		component->setLayoutManager(layout);
		return layout;
	}
}

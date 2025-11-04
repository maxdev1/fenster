// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/stack_layout_manager.hpp"
#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/json/json.hpp"
#include <sstream>
#include <map>

namespace fenster
{
	void StackLayoutManager::setHorizontal(bool horizontal)
	{
		component->setNumericProperty(FENSTER_UI_PROPERTY_LAYOUT_HORIZONTAL, horizontal ? 1 : 0);
	}

	void StackLayoutManager::setSpace(int space)
	{
		component->setNumericProperty(FENSTER_UI_PROPERTY_LAYOUT_SPACE, space);
	}

	void StackLayoutManager::setPadding(Insets insets)
	{
		std::map<std::string, JsonNode> data;
		data["top"] = JsonNode((double) insets.top);
		data["left"] = JsonNode((double) insets.left);
		data["bottom"] = JsonNode((double) insets.bottom);
		data["right"] = JsonNode((double) insets.right);

		JsonNode node(data);
		component->setStringProperty(FENSTER_UI_PROPERTY_LAYOUT_PADDING, Json::stringify(node));
	}

	StackLayoutManager* StackLayoutManager::create(Component* component)
	{
		auto layout = new StackLayoutManager(component);
		component->setLayoutManager(layout);
		return layout;
	}


}

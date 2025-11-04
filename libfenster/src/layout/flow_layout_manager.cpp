// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/flow_layout_manager.hpp"
#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/json/json.hpp"
#include <sstream>
#include <map>

namespace fenster
{
	void FlowLayoutManager::setPadding(fenster::Insets insets)
	{
		std::map<std::string, JsonNode> data;
		data["top"] = JsonNode((double) insets.top);
		data["left"] = JsonNode((double) insets.left);
		data["bottom"] = JsonNode((double) insets.bottom);
		data["right"] = JsonNode((double) insets.right);

		Json json;
		JsonNode node(data);
		component->setStringProperty(FENSTER_UI_PROPERTY_LAYOUT_PADDING, json.stringify(node));
	}

	FlowLayoutManager* FlowLayoutManager::create(Component* component)
	{
		auto layout = new FlowLayoutManager(component);
		component->setLayoutManager(layout);
		return layout;
	}


}

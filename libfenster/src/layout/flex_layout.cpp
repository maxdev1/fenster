// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/flex_layout.hpp"
#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/json/json.hpp"
#include <sstream>
#include <map>

namespace fenster
{
	void FlexLayout::setHorizontal(bool horizontal)
	{
		component->setNumericProperty(FENSTER_UI_PROPERTY_LAYOUT_HORIZONTAL, horizontal ? 1 : 0);
	}

	void FlexLayout::setSpace(int space)
	{
		component->setNumericProperty(FENSTER_UI_PROPERTY_LAYOUT_SPACE, space);
	}

	void FlexLayout::setPadding(Insets insets)
	{
		std::map<std::string, JsonNode> data;
		data["top"] = JsonNode((double) insets.top);
		data["left"] = JsonNode((double) insets.left);
		data["bottom"] = JsonNode((double) insets.bottom);
		data["right"] = JsonNode((double) insets.right);

		JsonNode node(data);
		component->setStringProperty(FENSTER_UI_PROPERTY_LAYOUT_PADDING, Json::stringify(node));
	}

	void FlexLayout::setComponentInfo(Component* child, float grow, float shrink, int basis)
	{
		std::map<std::string, JsonNode> data;
		data["component"] = JsonNode((double) child->getId());
		data["grow"] = JsonNode((double) grow);
		data["shrink"] = JsonNode((double) shrink);
		data["basis"] = JsonNode((double) basis);

		JsonNode node(data);
		component->setStringProperty(FENSTER_UI_PROPERTY_LAYOUT_COMPONENT_INFO, Json::stringify(node));
	}

	FlexLayout* FlexLayout::create(Component* component)
	{
		auto layout = new FlexLayout(component);
		component->setLayout(layout);
		return layout;
	}


}

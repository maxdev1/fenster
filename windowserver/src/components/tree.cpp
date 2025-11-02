// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/tree.hpp"
#include "components/tree_node.hpp"
#include "components/window.hpp"
#include "layout/stack_layout_manager.hpp"

#include <libjson/json.hpp>

tree_t::tree_t()
{
	auto layout = new stack_layout_manager_t();
	this->component_t::setLayoutManager(layout);
}

void tree_t::setModelFromJson(std::string& model)
{
	for(auto child: this->acquireChildren())
	{
		this->removeChild(child.component);
		delete child.component;
	}
	this->releaseChildren();

	g_json parser;
	auto json = parser.parse(model);

	if(!json.isObject())
	{
		platformLog("Tree model JSON was not an object");
		return;
	}
	auto jsonObject = json.asObject();

	auto rootNodes = jsonObject["rootNodes"];
	if(!rootNodes.isArray())
	{
		platformLog("Property 'rootNodes' in JSON model must be an array");
		return;
	}

	for(auto node: rootNodes.asArray())
	{
		auto nodeComponent = this->createNodeComponent(node);
		if(nodeComponent)
			this->addChild(nodeComponent);
	}
}

tree_node_t* tree_t::createNodeComponent(g_json_node& node)
{
	if(!node.isObject())
	{
		platformLog("Skipping non-object node in 'rootNodes'");
		return nullptr;
	}

	auto nodeObject = node.asObject();

	auto nodeComponent = new tree_node_t();

	auto titleValue = nodeObject["title"];
	if(titleValue.isString())
		nodeComponent->setTitle(titleValue.asString());

	// TODO Add ID

	auto childrenValue = nodeObject["children"];
	if(childrenValue.isArray())
	{
		for(auto child: childrenValue.asArray())
		{
			if(!child.isObject())
				continue;

			auto childComponent = this->createNodeComponent(child);
			if(childComponent)
				nodeComponent->addChild(childComponent);
		}
	}


	return nodeComponent;
}

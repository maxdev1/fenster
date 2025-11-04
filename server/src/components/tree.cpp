// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/tree.hpp"
#include "components/tree_node.hpp"
#include "components/window.hpp"
#include "layout/stack_layout_manager.hpp"
#include "libfenster/json/json.hpp"

namespace fensterserver
{
	Tree::Tree()
	{
		auto layout = new StackLayoutManager();
		this->Component::setLayoutManager(layout);
	}

	void Tree::setModelFromJson(std::string& model)
	{
		for(auto child: this->acquireChildren())
		{
			this->removeChild(child.component);
			delete child.component;
		}
		this->releaseChildren();

		auto json = fenster::Json::parse(model);

		if(!json.isObject())
		{
			fenster::platformLog("Tree model JSON was not an object");
			return;
		}
		auto jsonObject = json.asObject();

		auto rootNodes = jsonObject["rootNodes"];
		if(!rootNodes.isArray())
		{
			fenster::platformLog("Property 'rootNodes' in JSON model must be an array");
			return;
		}

		for(auto node: rootNodes.asArray())
		{
			auto nodeComponent = this->createNodeComponent(node);
			if(nodeComponent)
				this->addChild(nodeComponent);
		}
	}

	TreeNode* Tree::createNodeComponent(fenster::JsonNode& node)
	{
		if(!node.isObject())
		{
			fenster::platformLog("Skipping non-object node in 'rootNodes'");
			return nullptr;
		}

		auto nodeObject = node.asObject();

		auto nodeComponent = new TreeNode();

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
}

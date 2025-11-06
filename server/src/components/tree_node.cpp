// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/tree_node.hpp"
#include "components/window.hpp"

namespace fensterserver
{
	TreeNode::TreeNode()
	{
		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
	}

	void TreeNode::update()
	{
		auto preferred = getMinimumSize();

		for(auto child: acquireChildren())
		{
			if(!open && child.component != &label)
				continue;

			auto childPrefSize = child.component->getPreferredSize();
			preferred.height += childPrefSize.height;
			if(childPrefSize.width + 20 > preferred.width)
				preferred.width = childPrefSize.width + 20;
		}
		releaseChildren();

		setPreferredSize(preferred);

		markParentFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	void TreeNode::layout()
	{
		auto bounds = getBounds();
		bounds.x = 0;
		bounds.y = 0;
		bounds.height = 20;
		label.setBounds(bounds);

		if(open)
		{
			int y = 20;
			for(auto child: acquireChildren())
			{
				if(child.component == &label)
					continue;

				auto childPrefSize = child.component->getPreferredSize();
				bounds.x = 20;
				bounds.y = y;
				bounds.width = childPrefSize.width;
				bounds.height = childPrefSize.height;
				child.component->setBounds(bounds);

				y += childPrefSize.height;
			}
			releaseChildren();
		}
	}

	void TreeNode::setTitleInternal(std::string title)
	{
		this->label.setTitle(title);
	}

	std::string TreeNode::getTitle()
	{
		return this->label.getTitle();
	}

	Component* TreeNode::handleMouseEvent(MouseEvent& event)
	{
		Component* handledByChild = Component::handleMouseEvent(event);
		if(handledByChild)
			return handledByChild;

		if(event.type == fenster::MouseEventType::Press)
		{
			this->open = !this->open;
			markFor(COMPONENT_REQUIREMENT_ALL);
			return this;
		}

		return nullptr;
	}
}

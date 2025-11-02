// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "layout/stack_layout_manager.hpp"
#include "components/component.hpp"

namespace fensterserver
{
	/**
	 *
	 */
	void StackLayoutManager::layout()
	{
		if(component == nullptr)
			return;

		if(horizontal)
		{
			int x = 0;
			int highestHeight = 0;

			auto& children = component->acquireChildren();
			for(auto& childRef: children)
			{
				Component* child = childRef.component;
				if(!child->isVisible())
					continue;
				fenster::Dimension childSize = child->getEffectivePreferredSize();

				child->setBounds(fenster::Rectangle(x, 0, childSize.width, childSize.height));
				x += childSize.width;

				highestHeight = childSize.height > highestHeight ? childSize.height : highestHeight;
			}
			component->releaseChildren();

			component->setPreferredSize(fenster::Dimension(x, highestHeight));
		}
		else
		{
			int y = 0;
			int widestWidth = 0;

			auto& children = component->acquireChildren();
			for(auto& childRef: children)
			{
				Component* child = childRef.component;
				if(!child->isVisible())
					continue;
				fenster::Dimension childSize = child->getEffectivePreferredSize();

				child->setBounds(fenster::Rectangle(0, y, childSize.width, childSize.height));
				y += childSize.height;

				widestWidth = childSize.width > widestWidth ? childSize.width : widestWidth;
			}
			component->releaseChildren();

			component->setPreferredSize(fenster::Dimension(widestWidth, y));
		}
	}
}

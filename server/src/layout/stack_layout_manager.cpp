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
			int x = padding.left;
			int highestHeight = 0;

			auto& children = component->acquireChildren();
			bool first = true;
			for(auto& childRef: children)
			{
				if(first)
					first = false;
				else
					x += space;

				Component* child = childRef.component;
				if(!child->isVisible())
					continue;
				fenster::Dimension childSize = child->getEffectivePreferredSize();

				child->setBounds(fenster::Rectangle(x, padding.top, childSize.width, childSize.height));
				x += childSize.width;

				highestHeight = childSize.height > highestHeight ? childSize.height : highestHeight;
			}
			component->releaseChildren();

			component->setPreferredSize(
					fenster::Dimension(x + padding.right, padding.top + highestHeight + padding.bottom));
		}
		else
		{
			int y = padding.top;
			int widestWidth = 0;

			auto& children = component->acquireChildren();
			bool first = true;
			for(auto& childRef: children)
			{
				if(first)
					first = false;
				else
					y += space;

				Component* child = childRef.component;
				if(!child->isVisible())
					continue;
				fenster::Dimension childSize = child->getEffectivePreferredSize();

				child->setBounds(fenster::Rectangle(padding.left, y, childSize.width, childSize.height));
				y += childSize.height;

				widestWidth = childSize.width > widestWidth ? childSize.width : widestWidth;
			}
			component->releaseChildren();

			component->setPreferredSize(
					fenster::Dimension(padding.left + widestWidth + padding.right, y + padding.bottom));
		}
	}
}

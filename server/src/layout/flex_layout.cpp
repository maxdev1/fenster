// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/flex_layout.hpp"

namespace fensterserver
{
	void FlexLayout::setComponentInfo(Component* child, float grow, float shrink, int basis)
	{
		FlexInfo info;
		info.grow = grow;
		info.shrink = shrink;
		info.basis = basis;
		flexInfo[child] = info;
	}

	void FlexLayout::layout()
	{
		if(component == nullptr)
			return;

		fenster::Rectangle bounds = component->getBounds();
		bounds.x = padding.left;
		bounds.y = padding.top;
		bounds.width -= padding.left + padding.right;
		bounds.height -= padding.top + padding.bottom;

		auto& children = component->acquireChildren();
		int totalSpace = horizontal ? bounds.width : bounds.height;
		int totalFlexGrow = 0;
		int totalFixedSpace = 0;

		// Calculate required space for fixed-size
		for(auto& ref: children)
		{
			Component* child = ref.component;
			FlexInfo flex = flexInfo[child];

			int basis = (flex.basis >= 0)
				            ? flex.basis
				            : (horizontal ? child->getPreferredSize().width : child->getPreferredSize().height);
			totalFixedSpace += basis;
			totalFlexGrow += flex.grow;
		}

		// Calculate remaining space, considering the gaps
		int remainingSize = totalSpace - totalFixedSpace - (space * (children.size() - 1));

		// Distribute space
		int xOffset = bounds.x;
		int yOffset = bounds.y;

		for(auto& ref: children)
		{
			Component* child = ref.component;
			FlexInfo flex = flexInfo[child];

			int basis = (flex.basis >= 0)
				            ? flex.basis
				            : (horizontal ? child->getPreferredSize().width : child->getPreferredSize().height);
			int allocatedSize = basis;

			if(remainingSize > 0 && flex.grow > 0)
			{
				float proportion = flex.grow / static_cast<float>(totalFlexGrow);
				allocatedSize += static_cast<int>(proportion * remainingSize);
			}

			if(horizontal)
			{
				child->setBounds(fenster::Rectangle(xOffset, yOffset, allocatedSize, bounds.height));
				xOffset += allocatedSize + space; // Add the gap after each component
			}
			else
			{
				child->setBounds(fenster::Rectangle(xOffset, yOffset, bounds.width, allocatedSize));
				yOffset += allocatedSize + space; // Add the gap after each component
			}
		}

		component->releaseChildren();
	}
}

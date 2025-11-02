// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "layout/stack_layout_manager.hpp"
#include "components/component.hpp"

/**
 *
 */
void stack_layout_manager_t::layout()
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
			component_t* child = childRef.component;
			if(!child->isVisible())
				continue;
			g_dimension childSize = child->getEffectivePreferredSize();

			child->setBounds(g_rectangle(x, 0, childSize.width, childSize.height));
			x += childSize.width;

			highestHeight = childSize.height > highestHeight ? childSize.height : highestHeight;
		}
		component->releaseChildren();

		component->setPreferredSize(g_dimension(x, highestHeight));
	}
	else
	{
		int y = 0;
		int widestWidth = 0;

		auto& children = component->acquireChildren();
		for(auto& childRef: children)
		{
			component_t* child = childRef.component;
			if(!child->isVisible())
				continue;
			g_dimension childSize = child->getEffectivePreferredSize();

			child->setBounds(g_rectangle(0, y, childSize.width, childSize.height));
			y += childSize.height;

			widestWidth = childSize.width > widestWidth ? childSize.width : widestWidth;
		}
		component->releaseChildren();

		component->setPreferredSize(g_dimension(widestWidth, y));
	}
}

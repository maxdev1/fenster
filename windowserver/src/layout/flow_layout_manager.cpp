// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "layout/flow_layout_manager.hpp"
#include "components/component.hpp"

/**
 *
 */
void flow_layout_manager_t::layout()
{
	if(component == nullptr)
		return;


	g_rectangle bounds = component->getBounds();
	bounds.x = padding.left;
	bounds.y = padding.top;
	bounds.width -= padding.left + padding.right;
	bounds.height -= padding.top + padding.bottom;

	int x = bounds.x;
	int y = bounds.y;
	int rowHeight = 0;

	auto& children = component->acquireChildren();
	for(auto& childRef: children)
	{
		component_t* child = childRef.component;
		if(!child->isVisible())
			continue;
		g_dimension childSize = child->getEffectivePreferredSize();

		// Break when reaching end
		if(x + childSize.width > bounds.width)
		{
			x = bounds.x;
			y += rowHeight;
			rowHeight = 0;
		}

		// Set size
		child->setBounds(g_rectangle(x, y, childSize.width, childSize.height));
		x += childSize.width;

		if(childSize.height > rowHeight)
			rowHeight = childSize.height;
	}
	component->releaseChildren();

	component->setPreferredSize(g_dimension(bounds.width == 0 ? x : bounds.width, y + rowHeight));
}

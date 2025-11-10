// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/stack_layout.hpp"

namespace fensterserver
{
	/**
	 *
	 */
	void StackLayout::layout()
	{
		if(!component)
			return;

		fenster::Rectangle bounds = component->getBounds();
		bool horizontal = orientation == fenster::Orientation::Horizontal;
		int max = 0;
		int pos = horizontal ? padding.left : padding.top;
		bool first = true;

		auto& children = component->acquireChildren();
		for(auto& cr: children)
		{
			Component* child = cr.component;
			if(!child->isVisible())
				continue;

			if(!first)
				pos += space;
			first = false;

			auto size = child->getEffectivePreferredSize();

			if(horizontal)
			{
			    int usedHeight = bounds.height > 0 ? bounds.height - padding.top - padding.bottom : size.height;
				child->setBounds({pos, padding.top, size.width, usedHeight});
				pos += size.width;
				max = std::max(max, usedHeight);
			}
			else
			{
			    int usedWidth = bounds.width > 0 ? bounds.width - padding.left - padding.right : size.width;
				child->setBounds({padding.left, pos, usedWidth, size.height});
				pos += size.height;
				max = std::max(max, usedWidth);
			}
		}
		component->releaseChildren();

		if(horizontal)
		{
			component->setPreferredSize({
					pos + padding.right,
					padding.top + max + padding.bottom
			});
		}
		else
		{
			component->setPreferredSize({
					padding.left + max + padding.right,
					pos + padding.bottom
			});
		}
	}
}

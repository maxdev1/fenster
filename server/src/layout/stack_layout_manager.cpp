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
		if(!component)
			return;

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
				child->setBounds({pos, padding.top, size.width, size.height});
				pos += size.width;
				max = std::max(max, size.height);
			}
			else
			{
				child->setBounds({padding.left, pos, size.width, size.height});
				pos += size.height;
				max = std::max(max, size.width);
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

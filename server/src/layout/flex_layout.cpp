// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/flex_layout.hpp"

#include "components/scrollpane.hpp"

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

		bool horizontal = orientation == fenster::Orientation::Horizontal;

		fenster::Rectangle bounds = component->getBounds();

		bounds.x = padding.left;
		bounds.y = padding.top;
		bounds.width -= padding.left + padding.right;
		bounds.height -= padding.top + padding.bottom;

		auto& children = component->acquireChildren();

		float totalBasis = 0.f;
		float totalGrow = 0.f;
		float totalShrink = 0.f;

		for(auto& ref: children)
		{
			Component* child = ref.component;
			auto info = flexInfo[child];

			fenster::Dimension pref = child->getPreferredSize();
			int childPref = horizontal ? pref.width : pref.height;

			int basis = (info.basis >= 0) ? info.basis : childPref;
			totalBasis += basis;
			totalGrow  += info.grow;
			totalShrink += info.shrink;

			info.basis = basis;
			flexInfo[child] = info;
		}

		int mainSize = horizontal ? bounds.width : bounds.height;
		int crossSize = horizontal ? bounds.height : bounds.width;

		float remaining = mainSize - totalBasis;

		int pos = horizontal ? bounds.x : bounds.y;
		int totalMainUsed = 0;

		for(auto& ref: children)
		{
			Component* child = ref.component;
			auto info = flexInfo[child];

			float size = info.basis;

			if(remaining > 0 && totalGrow > 0)
			{
				size += (info.grow / totalGrow) * remaining;
			} else if(remaining < 0 && totalShrink > 0)
			{
				float overflow = -remaining;
				size -= (info.shrink / totalShrink) * overflow;
			}

			int finalSize = std::max(0, (int) size);

			if(horizontal)
			{
				child->setBounds(fenster::Rectangle(pos, bounds.y, finalSize, crossSize));
				pos += finalSize;
			} else
			{
				child->setBounds(fenster::Rectangle(bounds.x, pos, crossSize, finalSize));
				pos += finalSize;
			}

			totalMainUsed += finalSize;
		}

		component->releaseChildren();

		int endX = horizontal ? totalMainUsed : crossSize;
		int endY = horizontal ? crossSize : totalMainUsed;
		component->setPreferredSize(fenster::Dimension(endX, endY));
	}
}

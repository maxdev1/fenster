// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/flow_layout.hpp"

#include "components/scrollpane.hpp"

namespace fensterserver {
	void FlowLayout::layout() {
		if (component == nullptr)
			return;

		fenster::Rectangle bounds = component->getBounds();
		bounds.x = padding.left;
		bounds.y = padding.top;
		bounds.width -= padding.left + padding.right;
		bounds.height -= padding.top + padding.bottom;

		int curX = bounds.x;
		int curY = bounds.y;
		int rowHeight = 0;

		auto& children = component->acquireChildren();

		for (size_t i = 0; i < children.size(); ++i) {
			Component* child = children[i].component;
			if (!child->isVisible())
				continue;

			fenster::Dimension childSize = child->getEffectivePreferredSize();

			bool firstInRow = (curX == bounds.x);

			int neededWidth = childSize.width;
			if (!firstInRow)
				neededWidth += horizontalSpace;

			if (curX + neededWidth > bounds.x + bounds.width) {
				curX = bounds.x;
				curY += rowHeight;
				if (rowHeight > 0)
					curY += verticalSpace;

				rowHeight = 0;
				firstInRow = true;
			}

			if (!firstInRow)
				curX += horizontalSpace;

			child->setBounds(fenster::Rectangle(curX, curY,
			                                    childSize.width, childSize.height));

			curX += childSize.width;
			rowHeight = std::max(rowHeight, childSize.height);
		}

		component->releaseChildren();

		int totalHeight = curY + rowHeight;
		if (rowHeight > 0 && curY > bounds.y)
			totalHeight += verticalSpace;

		int usedWidth = (curX > bounds.x) ? curX - bounds.x : 0;

		component->setPreferredSize(
			fenster::Dimension(bounds.width == 0 ? usedWidth : bounds.width,
			                   totalHeight));
	}
}

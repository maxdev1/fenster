// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/grid_layout.hpp"

#include <vector>

#include "components/scrollpane.hpp"

namespace fensterserver {
	void GridLayout::layout() {
		if (component == nullptr || columns <= 0)
			return;

		auto& children = component->acquireChildren();
		auto bounds = component->getBounds();

		// Compute available area
		int availableWidth = bounds.width - (padding.left + padding.right);
		int availableHeight = bounds.height - (padding.top + padding.bottom);

		// Compute cell dimensions
		int actualRows = (int) children.size() / columns;
		if ((int) children.size() % columns != 0)
			actualRows++;

		// Calculate preferred cell sizes if not filling
		int largestPrefWidth = 0;
		int largestPrefHeight = 0;
		for (auto& ref: children) {
			auto childSize = ref.component->getEffectivePreferredSize();
			largestPrefWidth = std::max(largestPrefWidth, childSize.width);
			largestPrefHeight = std::max(largestPrefHeight, childSize.height);
		}

		int cellWidth;
		if (component->layoutPolicyHorizontal == LayoutingPolicy::Stretch || largestPrefWidth == 0) {
			cellWidth = (availableWidth - (verticalSpace * (columns - 1))) / columns;
		} else {
			cellWidth = largestPrefWidth;
		}
		int cellHeight;
		if (component->layoutPolicyVertical == LayoutingPolicy::Stretch || largestPrefHeight == 0) {
			cellHeight = (availableHeight - (horizontalSpace * (actualRows - 1))) / actualRows;
		} else {
			cellHeight = largestPrefHeight;
		}

		// Place each child into the grid
		int currentRow = 0;
		int currentColumn = 0;
		int endX = 0;
		int endY = 0;

		for (auto& childRef: children) {
			Component* child = childRef.component;
			if (!child->isVisible())
				continue;

			int x = padding.left + currentColumn * (cellWidth + verticalSpace);
			int y = padding.top + currentRow * (cellHeight + horizontalSpace);

			fenster::Rectangle childBounds(x, y, cellWidth, cellHeight);
			child->setBounds(childBounds);

			auto childBoundsEnd = childBounds.getEnd();
			endX = std::max(childBoundsEnd.x - padding.left, endX);
			endY = std::max(childBoundsEnd.y - padding.top, endY);

			// Advance column
			currentColumn++;
			if (currentColumn >= columns) {
				currentColumn = 0;
				currentRow++;
			}
		}

		component->releaseChildren();

		component->setPreferredSize(fenster::Dimension(endX, endY));
	}
}

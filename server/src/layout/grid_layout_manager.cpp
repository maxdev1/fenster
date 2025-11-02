// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "layout/grid_layout_manager.hpp"
#include "components/component.hpp"

#include <vector>

namespace fensterserver
{
	void GridLayoutManager::layout()
	{
		if(component == nullptr)
			return;

		if(columns <= 0)
		{
			fenster::platformLog("grid layout must have a defined number of columns");
			return;
		}

		auto& children = component->acquireChildren();

		// Get component bounds and apply padding
		fenster::Rectangle bounds = component->getBounds();
		bounds.x = padding.left;
		bounds.y = padding.top;
		bounds.width = std::max(bounds.width - padding.left - padding.right, 0);
		bounds.height = std::max(bounds.height - padding.top - padding.bottom, 0);

		// Compute cell dimensions
		int actualRows = (int) children.size() / columns;
		int columnWidth = (columns > 0) ? (bounds.width - (columns - 1) * colSpace) / columns : 0;
		int rowHeight = (rows > 0)
							? (bounds.height - (rows - 1) * rowSpace) / rows
							: (bounds.height - (actualRows - 1) * rowSpace) / actualRows;

		// If parent has no width or height, calculate expected size from children
		if(bounds.width == 0 || bounds.height == 0)
		{
			for(auto& ref: children)
			{
				auto childSize = ref.component->getEffectivePreferredSize();
				if(bounds.width == 0)
					columnWidth = std::max(columnWidth, childSize.width);
				if(bounds.height == 0)
					rowHeight = std::max(rowHeight, childSize.height);
			}
		}

		// Place each child into the grid
		int currentRow = 0;
		int currentColumn = 0;

		for(auto& childRef: children)
		{
			Component* child = childRef.component;
			if(!child->isVisible())
				continue;
			auto childSize = child->getPreferredSize();

			// Use calculated cell size unless zero (in which case use child's preferred size)
			int cellWidth = (columnWidth > 0) ? columnWidth : childSize.width;
			int cellHeight = (rowHeight > 0) ? rowHeight : childSize.height;

			// Calculate position based on cell index and spacing
			int x = bounds.x + currentColumn * (columnWidth + colSpace);
			int y = bounds.y + currentRow * (rowHeight + rowSpace);

			child->setBounds(fenster::Rectangle(x, y, cellWidth, cellHeight));

			// Advance column
			currentColumn++;
			if(currentColumn >= columns)
			{
				currentColumn = 0;
				currentRow++;
			}
		}

		component->releaseChildren();

		// Set preferred size if parent has no bounds
		if(bounds.width == 0 || bounds.height == 0)
		{
			int contentWidth = columns * columnWidth + (columns - 1) * colSpace + padding.left + padding.right;
			int contentHeight = (rows > 0)
									? rows * rowHeight + (rows - 1) * rowSpace + padding.top + padding.bottom
									: (currentRow + 1) * rowHeight;

			component->setPreferredSize(fenster::Dimension(contentWidth, contentHeight));
		}
	}
}

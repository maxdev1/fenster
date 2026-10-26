// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/grid_layout.hpp"
#include <vector>

namespace fensterserver
{
void GridLayout::layout()
{
    if(!component || columns <= 0) return;
    auto& children = component->acquireChildren();
    std::vector<Component*> visible;
    int prefWidth = 0, prefHeight = 0;
    for(auto& ref : children)
    {
        if(!ref.component->isVisible()) continue;
        visible.push_back(ref.component);
        auto size = ref.component->getEffectivePreferredSize();
        prefWidth = std::max(prefWidth, size.width); prefHeight = std::max(prefHeight, size.height);
    }
    if(visible.empty())
    {
        component->releaseChildren();
        component->setPreferredSize({padding.left + padding.right, padding.top + padding.bottom});
        return;
    }
    auto bounds = component->getBounds();
    int rowCount = std::max(rows, (int(visible.size()) + columns - 1) / columns);
    int width = std::max(0, bounds.width - padding.left - padding.right - horizontalSpace * (columns - 1));
    int height = std::max(0, bounds.height - padding.top - padding.bottom - verticalSpace * (rowCount - 1));
    bool stretchX = component->layoutPolicyHorizontal == LayoutingPolicy::Stretch || prefWidth == 0;
    bool stretchY = component->layoutPolicyVertical == LayoutingPolicy::Stretch || prefHeight == 0;
    for(size_t i = 0; i < visible.size(); ++i)
    {
        int col = i % columns, row = i / columns;
        // Distribute remainder pixels instead of leaving gaps at the right/bottom.
        int x = stretchX ? col * width / columns : col * prefWidth;
        int y = stretchY ? row * height / rowCount : row * prefHeight;
        int w = stretchX ? (col + 1) * width / columns - x : prefWidth;
        int h = stretchY ? (row + 1) * height / rowCount - y : prefHeight;
        visible[i]->setBounds({padding.left + x + col * horizontalSpace, padding.top + y + row * verticalSpace, w, h});
    }
    component->releaseChildren();
    int usedColumns = std::min(columns, int(visible.size()));
    component->setPreferredSize({padding.left + padding.right + usedColumns * prefWidth + (usedColumns - 1) * horizontalSpace,
        padding.top + padding.bottom + rowCount * prefHeight + (rowCount - 1) * verticalSpace});
}
}

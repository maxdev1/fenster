// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/flow_layout.hpp"

namespace fensterserver
{
void FlowLayout::layout()
{
    if(!component) return;
    int available = std::max(0, component->getBounds().width - padding.left - padding.right);
    int x = 0, y = 0, rowHeight = 0, usedWidth = 0;
    auto& children = component->acquireChildren();
    for(auto& ref : children)
    {
        auto child = ref.component;
        if(!child->isVisible()) continue;
        auto size = child->getEffectivePreferredSize();
        if(x > 0 && x + horizontalSpace + size.width > available)
        {
            x = 0; y += rowHeight + verticalSpace; rowHeight = 0;
        }
        if(x > 0) x += horizontalSpace;
        child->setBounds({padding.left + x, padding.top + y, size.width, size.height});
        x += size.width; usedWidth = std::max(usedWidth, x); rowHeight = std::max(rowHeight, size.height);
    }
    component->releaseChildren();
    component->setPreferredSize({padding.left + usedWidth + padding.right, padding.top + y + rowHeight + padding.bottom});
}
}

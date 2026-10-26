// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/fill_layout.hpp"

namespace fensterserver
{
void FillLayout::layout()
{
    if(!component) return;
    auto bounds = component->getBounds();
    int width = 0, height = 0;
    for(auto& ref : component->acquireChildren())
    {
        if(!ref.component->isVisible()) continue;
        auto preferred = ref.component->getEffectivePreferredSize();
        width = std::max(width, preferred.width); height = std::max(height, preferred.height);
        ref.component->setBounds({padding.left, padding.top, std::max(0, bounds.width - padding.left - padding.right),
            std::max(0, bounds.height - padding.top - padding.bottom)});
    }
    component->releaseChildren();
    component->setPreferredSize({width + padding.left + padding.right, height + padding.top + padding.bottom});
}
}

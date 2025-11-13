// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/fill_layout.hpp"

#include <vector>

namespace fensterserver {
    void FillLayout::layout() {
        if (component == nullptr)
            return;

        auto bounds = component->getBounds();
        for (auto child: component->acquireChildren()) {
            child.component->setBounds(fenster::Rectangle(0, 0, bounds.width, bounds.height));
        }
        component->releaseChildren();
    }
}

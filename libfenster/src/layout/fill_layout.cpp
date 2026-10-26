// fenster — lightweight window server and UI toolkit
// Copyright (c) 2026 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/fill_layout.hpp"
#include "libfenster/components/component.hpp"

namespace fenster
{
FillLayout* FillLayout::create(Component* component)
{
    auto layout = new FillLayout(component);
    component->setLayout(layout);
    return layout;
}
}

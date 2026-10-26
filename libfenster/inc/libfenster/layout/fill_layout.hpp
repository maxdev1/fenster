// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LAYOUT_FILLLAYOUT
#define LIBFENSTER_LAYOUT_FILLLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"

namespace fenster
{
class FillLayout : public Layout, public PaddingSupport
{
    explicit FillLayout(Component* component) : Layout(component), PaddingSupport(component) {}
public:
    static FillLayout* create(Component* component);
    LayoutType getType() override { return LayoutType::Fill; }
};
}
#endif

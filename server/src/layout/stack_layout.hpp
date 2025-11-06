// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_STACKLAYOUT
#define FENSTER_SERVER_LAYOUT_STACKLAYOUT

#include "layout.hpp"
#include "support/orientation_support.hpp"
#include "support/padding_support.hpp"
#include "support/single_spacing_support.hpp"

namespace fensterserver
{
    class StackLayout : public Layout, public PaddingSupport, public SingleSpacingSupport, public OrientationSupport
    {
    public:
        StackLayout() = default;

        explicit StackLayout(int space): SingleSpacingSupport(space)
        {
        }

        void layout() override;
    };
}

#endif

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

        bool debug = false;

        explicit StackLayout(int space): SingleSpacingSupport(space)
        {
        }

        explicit StackLayout(fenster::Orientation orientation): OrientationSupport(orientation)
        {
        }

        explicit StackLayout(fenster::Insets insets): PaddingSupport(insets)
        {
        }

        explicit StackLayout(fenster::Orientation orientation, fenster::Insets insets): OrientationSupport(orientation), PaddingSupport(insets)
        {
        }

        explicit StackLayout(int space, fenster::Insets insets): SingleSpacingSupport(space), PaddingSupport(insets)
        {
        }

        explicit StackLayout(int space, fenster::Orientation orientation): SingleSpacingSupport(space), OrientationSupport(orientation)
        {
        }

        explicit StackLayout(int space, fenster::Orientation orientation, fenster::Insets insets): PaddingSupport(insets), SingleSpacingSupport(space), OrientationSupport(orientation)
        {
        }

        void layout() override;
    };
}

#endif

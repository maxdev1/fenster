// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FLEXLAYOUT
#define FENSTER_SERVER_LAYOUT_FLEXLAYOUT

#include <unordered_map>
#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/single_spacing_support.hpp"
#include "support/orientation_support.hpp"

namespace fensterserver
{
    struct FlexInfo
    {
        float grow;
        float shrink;
        int basis;
    };

    class FlexLayout : public Layout, public PaddingSupport, public SingleSpacingSupport, public OrientationSupport
    {
        std::unordered_map<Component*, FlexInfo> flexInfo{};

    public:
        void layout() override;

        void setComponentInfo(Component* child, float grow, float shrink, int basis);
    };
}

#endif

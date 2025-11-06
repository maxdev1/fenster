// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FLOWLAYOUT
#define FENSTER_SERVER_LAYOUT_FLOWLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/single_spacing_support.hpp"

namespace fensterserver
{
    class FlowLayout : public Layout, public PaddingSupport, public SingleSpacingSupport
    {
    public:
        void layout() override;
    };
}

#endif

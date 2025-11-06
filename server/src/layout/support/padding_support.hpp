// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_PADDINGSUPPORT
#define FENSTER_SERVER_LAYOUT_PADDINGSUPPORT

#include <libfenster/metrics/insets.hpp>

namespace fensterserver
{
    class PaddingSupport
    {
    protected:
        fenster::Insets padding = fenster::Insets(0, 0, 0, 0);

    public:
        PaddingSupport() = default;

        explicit PaddingSupport(fenster::Insets padding) : padding(padding)
        {
        }

        void setPadding(fenster::Insets padding)
        {
            this->padding = padding;
        }

        fenster::Insets getPadding()
        {
            return this->padding;
        }
    };
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_ORIENTATIONSUPPORT
#define FENSTER_SERVER_LAYOUT_ORIENTATIONSUPPORT

#include "libfenster/layout/support/orientation_support.hpp"

namespace fensterserver
{
    class OrientationSupport
    {
    protected:
        fenster::Orientation orientation = fenster::Orientation::Vertical;

    public:
        OrientationSupport() = default;

        explicit OrientationSupport(fenster::Orientation orientation) : orientation(orientation)
        {
        }

        void setOrientation(fenster::Orientation orientation)
        {
            this->orientation = orientation;
        }

        fenster::Orientation getOrientation() const
        {
            return this->orientation;
        }
    };
}

#endif

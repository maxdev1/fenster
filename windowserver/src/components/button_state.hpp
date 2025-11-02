// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_BUTTONSTATE
#define FENSTER_SERVER_COMPONENTS_BUTTONSTATE

namespace fensterserver
{
    struct ButtonState
    {
        ButtonState() : hovered(false), pressed(false)
        {
        }

        bool hovered;
        bool pressed;
    };
}

#endif

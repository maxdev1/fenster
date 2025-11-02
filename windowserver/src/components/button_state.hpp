// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_BUTTONSTATE__
#define __WINDOWSERVER_COMPONENTS_BUTTONSTATE__

struct button_state_t
{
    button_state_t() : hovered(false), pressed(false)
    {
    }

    bool hovered;
    bool pressed;
};

#endif

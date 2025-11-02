// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TEXT_CARETDIRECTION
#define FENSTER_SERVER_COMPONENTS_TEXT_CARETDIRECTION

#include <stdint.h>

namespace fensterserver
{
    enum class CaretDirection : uint8_t
    {
        LEFT,
        RIGHT
    };
}

#endif

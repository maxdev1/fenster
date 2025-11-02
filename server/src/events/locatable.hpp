// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_EVENTS_LOCATABLE
#define FENSTER_SERVER_EVENTS_LOCATABLE

#include <libfenster/metrics/point.hpp>

namespace fensterserver
{
    class Component;

    class Locatable
    {
    public:
        fenster::Point position;
        fenster::Point screenPosition;
    };
}

#endif

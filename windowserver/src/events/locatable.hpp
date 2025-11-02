// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_EVENTS_LOCATABLE__
#define __WINDOWSERVER_EVENTS_LOCATABLE__

#include <libwindow/metrics/point.hpp>

class component_t;

class locatable_t
{
  public:
    g_point position;
    g_point screenPosition;
};

#endif

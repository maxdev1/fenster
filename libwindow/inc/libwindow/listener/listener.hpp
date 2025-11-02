// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_LISTENER
#define LIBWINDOW_LISTENER

#include "libwindow/interface.hpp"

class g_listener
{
public:
    virtual ~g_listener() = default;

    virtual void process(g_ui_component_event_header* header) = 0;
};

#endif

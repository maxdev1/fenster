// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_INTERFACE_PROCESSREGISTRY__
#define __WINDOWSERVER_INTERFACE_PROCESSREGISTRY__

#include <libwindow/interface.hpp>

class process_registry_t
{
public:
    static void bind(SYS_TID_T pid, SYS_TID_T eventDispatcher);
    static SYS_TID_T get(SYS_TID_T);
    static void cleanup_process(SYS_TID_T pid);
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_PROCESSREGISTRY
#define FENSTER_SERVER_PROCESSREGISTRY

#include <libfenster/interface.hpp>

namespace fensterserver
{
    class ProcessRegistry
    {
    public:
        static void bind(SYS_TID_T pid, SYS_TID_T eventDispatcher);
        static SYS_TID_T get(SYS_TID_T);
        static void cleanup_process(SYS_TID_T pid);
    };
}

#endif

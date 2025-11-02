// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_INTERFACE_APPLICATIONEXITCLEANUPHANDLER
#define FENSTER_SERVER_INTERFACE_APPLICATIONEXITCLEANUPHANDLER

#include "platform/platform.hpp"

namespace fensterserver
{
    class ApplicationExitCleanupHandler
    {
        SYS_TID_T pid;

    public:
        ApplicationExitCleanupHandler(SYS_TID_T pid) : pid(pid)
        {
        }

        virtual ~ApplicationExitCleanupHandler() = default;

        virtual void run();
    };

    void interfaceApplicationExitCleanupThread(ApplicationExitCleanupHandler* handler);
}

#endif

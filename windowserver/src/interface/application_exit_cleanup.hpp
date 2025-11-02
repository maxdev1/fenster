// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_INTERFACE_APPLICATIONEXITCLEANUP__
#define __WINDOWSERVER_INTERFACE_APPLICATIONEXITCLEANUP__

#include "interface/interface_receiver.hpp"
#include "platform/platform.hpp"

class application_exit_cleanup_handler_t
{
    SYS_TID_T pid;

public:
    application_exit_cleanup_handler_t(SYS_TID_T pid) : pid(pid)
    {
    }

    virtual ~application_exit_cleanup_handler_t() = default;

    virtual void run();
};

void interfaceApplicationExitCleanupThread(application_exit_cleanup_handler_t* handler);

#endif

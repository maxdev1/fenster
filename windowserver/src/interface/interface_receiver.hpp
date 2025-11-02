// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_INTERFACE_INTERFACERECEIVER
#define FENSTER_SERVER_INTERFACE_INTERFACERECEIVER

namespace fensterserver
{
    void interfaceReceiverThread();
    void interfaceReceiverProcessCommand(void* requestMessage);
}

#endif

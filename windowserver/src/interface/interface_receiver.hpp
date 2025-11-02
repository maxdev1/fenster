// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __INTERFACE_COMMAND_MESSAGE_RECEIVER__
#define __INTERFACE_COMMAND_MESSAGE_RECEIVER__

void interfaceReceiverThread();
void interfaceReceiverProcessCommand(void* requestMessage);

#endif

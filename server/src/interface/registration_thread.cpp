// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <libfenster/interface.hpp>

#include "interface/application_exit_cleanup.hpp"
#include "interface/interface_receiver.hpp"
#include "registration_thread.hpp"

#include "process_registry.hpp"

namespace fensterserver
{
	void interfaceRegistrationThread()
	{
		if(!fenster::platformRegisterTaskIdentifier(G_UI_REGISTRY_NAME))
		{
			fenster::platformLog("failed to register as \"%s\"", G_UI_REGISTRY_NAME);
			return;
		}

		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(fenster::CommandApplicationInitializeRequest);
		uint8_t buf[buflen];

		SYS_TID_T receiverTid = fenster::platformCreateThread((void*) &interfaceReceiverThread);
		while(true)
		{
			if(fenster::platformReceiveMessage(buf, buflen, SYS_TX_NONE) == SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				auto body = (fenster::CommandApplicationInitializeRequest*) SYS_MESSAGE_CONTENT(buf);

				ProcessRegistry::bind(fenster::platformGetPidForTid(body->event_dispatcher), body->event_dispatcher);

				fenster::platformCreateThreadWithData((void*) &interfaceApplicationExitCleanupThread,
								new ApplicationExitCleanupHandler(fenster::platformGetPidForTid(SYS_MESSAGE_SENDER(buf))));

				fenster::CommandApplicationInitializeResponse response;
				response.header.id = FENSTER_PROTOCOL_INITIALIZATION;
				response.status = FENSTER_PROTOCOL_SUCCESS;
				response.window_server_delegate = receiverTid;
				platformSendMessage(SYS_MESSAGE_SENDER(buf), &response, sizeof(fenster::CommandApplicationInitializeResponse),
								 SYS_MESSAGE_TRANSACTION(buf));
			}
		}
	}
}

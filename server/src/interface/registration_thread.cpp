// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <libfenster/interface.hpp>

#include "interface/application_exit_cleanup.hpp"
#include "interface/interface_receiver.hpp"
#include "registration_thread.hpp"

#include "process_registry.hpp"
#include "component_registry.hpp"

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
				if(SYS_MESSAGE_LENGTH(buf) != sizeof(fenster::CommandApplicationInitializeRequest))
					continue;
				auto body = (fenster::CommandApplicationInitializeRequest*) SYS_MESSAGE_CONTENT(buf);
				ComponentRegistry::Access components;
				auto pid = fenster::platformGetPidForTid(SYS_MESSAGE_SENDER(buf));
				if(body->header.id != fenster::ProtocolCommandId::Initialization || pid == SYS_TID_NONE ||
				   pid != fenster::platformGetPidForTid(body->event_dispatcher))
					continue;
				ProcessRegistry::bind(pid, body->event_dispatcher);
				auto handler = new ApplicationExitCleanupHandler(pid);
				bool initialized = receiverTid != SYS_TID_NONE && fenster::platformCreateThreadWithData(
						(void*) &interfaceApplicationExitCleanupThread, handler) != SYS_TID_NONE;
				if(!initialized)
				{
					delete handler;
					ProcessRegistry::cleanup_process(pid);
				}
				auto ids = initialized ? ComponentRegistry::reserveIds(pid) : fenster::ComponentIdRange{};
				if(initialized && !ids.count)
				{
					initialized = false;
					ProcessRegistry::cleanup_process(pid);
				}
				components.release();

				fenster::CommandApplicationInitializeResponse response;
				response.header.id = fenster::ProtocolCommandId::Initialization;
				response.status = initialized ? fenster::ProtocolStatus::Success : fenster::ProtocolStatus::Error;
				response.window_server_delegate = initialized ? receiverTid : SYS_TID_NONE;
				response.component_ids = ids;
				platformSendMessage(SYS_MESSAGE_SENDER(buf), &response, sizeof(fenster::CommandApplicationInitializeResponse),
								 SYS_MESSAGE_TRANSACTION(buf));
			}
		}
	}
}

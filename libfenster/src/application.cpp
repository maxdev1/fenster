// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <cstdio>

#include "libfenster/components/canvas.hpp"
#include "libfenster/components/component.hpp"
#include "libfenster/application.hpp"
#include "libfenster/component_update_batch.hpp"
#include "libfenster/platform/platform.hpp"

namespace fenster
{
	/**
	 * Global ready indicator
	 */
	bool ApplicationInitialized = false;

	/**
	 * ID of the window server interface receiver task
	 */
	SYS_TID_T DelegateTaskId = SYS_TID_NONE;

	/**
	 * Our local event dispatcher task ID
	 */
	SYS_TID_T EventDispatcherTaskId = SYS_TID_NONE;

	/**
	 * Opens a connection to the window server.
	 */
	ApplicationOpenStatus Application::open()
	{
		// check if already open
		if(ApplicationInitialized)
		{
			return ApplicationOpenStatus::Existing;
		}

		// get window managers id
		SYS_TID_T windowserverRegistryTask = platformWaitForServer();
		if(windowserverRegistryTask == SYS_TID_NONE)
		{
			platformLog("failed to retrieve task id of window server with identifier '%s'", G_UI_REGISTRY_NAME);
			return ApplicationOpenStatus::CommunicationFailed;
		}

		// start event dispatcher
		EventDispatcherTaskId = platformCreateThread((void*) &eventDispatchThread);

		// send initialization request
		SYS_TX_T init_tx = platformCreateMessageTransaction();

		CommandApplicationInitializeRequest request;
		request.header.id = fenster::ProtocolCommandId::Initialization;
		request.event_dispatcher = EventDispatcherTaskId;
		if(!sendUiMessage(windowserverRegistryTask, &request, sizeof(CommandApplicationInitializeRequest), init_tx))
			return ApplicationOpenStatus::CommunicationFailed;
		platformYieldTo(DelegateTaskId);

		// receive initialization response
		uint8_t buf[SYS_MESSAGE_HEADER_SIZE + sizeof(CommandApplicationInitializeResponse)];
		if(platformReceiveMessage(buf, sizeof(buf), init_tx) != SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			platformLog("failed to communicate with the window server");
			return ApplicationOpenStatus::CommunicationFailed;
		}

		// check response
		auto response = (CommandApplicationInitializeResponse*) SYS_MESSAGE_CONTENT(buf);
		if(response->status != ProtocolStatus::Success || !ComponentUpdateBatch::addIdRange(response->component_ids))
		{
			platformLog("failed to open UI");
			return ApplicationOpenStatus::Error;
		}

		// mark UI as ready
		ApplicationInitialized = true;
		DelegateTaskId = response->window_server_delegate;
		return ApplicationOpenStatus::Success;
	}

	/**
	 *
	 */
	bool Application::addListener(ComponentId id, ComponentEventType eventType)
	{
		if(auto batch = ComponentUpdateBatch::current())
			return batch->registerListener(id, eventType);
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandAddListenerRequest request;
		request.header.id = fenster::ProtocolCommandId::AddListener;
		request.id = id;
		request.target_thread = EventDispatcherTaskId;
		request.event_type = eventType;
		if(!sendUiMessage(DelegateTaskId, &request, sizeof(CommandAddListenerRequest), tx))
			return false;
		platformYieldTo(DelegateTaskId);

		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandAddListenerResponse);
		uint8_t buffer[bufferSize];
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandAddListenerResponse*) SYS_MESSAGE_CONTENT(buffer);
			return response->status == ProtocolStatus::Success;
		}
		return false;
	}

	/**
	 *
	 */
	void Application::eventDispatchThread()
	{
		size_t bufLen = G_UI_MAXIMUM_MESSAGE_SIZE;
		auto buf = new uint8_t[bufLen];

		while(true)
		{
			auto stat = platformReceiveMessage(buf, bufLen, SYS_TX_NONE);
			if(stat == SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				// event message
				auto event_header = (ComponentEventHeader*) SYS_MESSAGE_CONTENT(buf);
				Component* component = ComponentRegistry::get(event_header->component_id);

				if(component == nullptr)
				{
					platformLog("event received for unknown component %i", event_header->component_id);
					continue;
				}

				component->handle(event_header);
			}
			else
			{
				platformLog("something went wrong when receiving an event, status code: %i", stat);
			}
		}

		delete buf;
	}

	/**
	 *
	 */
	bool Application::registerDesktopCanvas(Canvas* c)
	{
		if(!ApplicationInitialized)
		{
			return false;
		}

		SYS_TX_T tx = platformCreateMessageTransaction();

		// send registration request
		CommandRegisterDesktopCanvasRequest request;
		request.header.id = fenster::ProtocolCommandId::RegisterDesktopCanvas;
		request.canvas_id = c->getId();
		request.target_thread = EventDispatcherTaskId;
		if(!sendUiMessage(DelegateTaskId, &request, sizeof(CommandRegisterDesktopCanvasRequest), tx))
			return false;
		platformYieldTo(DelegateTaskId);

		// read response
		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandRegisterDesktopCanvasResponse);
		uint8_t buf[buflen];

		bool success = false;
		if(platformReceiveMessage(buf, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandRegisterDesktopCanvasResponse*) SYS_MESSAGE_CONTENT(buf);
			success = (response->status == ProtocolStatus::Success);
		}
		return success;
	}

	/**
	 *
	 */
	bool Application::getScreenDimension(Dimension& out)
	{
		if(!ApplicationInitialized)
		{
			return false;
		}

		SYS_TX_T tx = platformCreateMessageTransaction();

		// send request
		CommandGetScreenDimensionRequest request;
		request.header.id = fenster::ProtocolCommandId::GetScreenDimension;
		if(!sendUiMessage(DelegateTaskId, &request, sizeof(CommandGetScreenDimensionRequest), tx))
			return false;
		platformYieldTo(DelegateTaskId);

		// read response
		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandGetScreenDimensionResponse);
		uint8_t* buffer = new uint8_t[bufferSize];

		bool success = false;
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			CommandGetScreenDimensionResponse* response = (CommandGetScreenDimensionResponse*) SYS_MESSAGE_CONTENT(buffer);
			out = response->size;
			success = true;
		}

		return success;
	}
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <cstdio>

#include "libfenster/canvas.hpp"
#include "libfenster/component.hpp"
#include "libfenster/application.hpp"
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
			return FENSTER_APPLICATION_STATUS_EXISTING;
		}

		// get window managers id
		SYS_TID_T windowserverRegistryTask = platformWaitForServer();
		if(windowserverRegistryTask == SYS_TID_NONE)
		{
			platformLog("failed to retrieve task id of window server with identifier '%s'", G_UI_REGISTRY_NAME);
			return FENSTER_APPLICATION_STATUS_COMMUNICATION_FAILED;
		}

		// start event dispatcher
		EventDispatcherTaskId = platformCreateThread((void*) &eventDispatchThread);

		// send initialization request
		SYS_TX_T init_tx = platformCreateMessageTransaction();

		CommandApplicationInitializeRequest request;
		request.header.id = FENSTER_PROTOCOL_INITIALIZATION;
		request.event_dispatcher = EventDispatcherTaskId;
		platformSendMessage(windowserverRegistryTask, &request, sizeof(CommandApplicationInitializeRequest), init_tx);
		platformYieldTo(DelegateTaskId);

		// receive initialization response
		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandApplicationInitializeResponse);
		uint8_t* buf[buflen];
		if(platformReceiveMessage(buf, buflen, init_tx) != SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			platformLog("failed to communicate with the window server");
			return FENSTER_APPLICATION_STATUS_COMMUNICATION_FAILED;
		}

		// check response
		auto response = (CommandApplicationInitializeResponse*) SYS_MESSAGE_CONTENT(buf);
		if(response->status != FENSTER_PROTOCOL_SUCCESS)
		{
			platformLog("failed to open UI");
			return FENSTER_APPLICATION_STATUS_FAILED;
		}

		// mark UI as ready
		ApplicationInitialized = true;
		DelegateTaskId = response->window_server_delegate;
		return FENSTER_APPLICATION_STATUS_SUCCESSFUL;
	}

	/**
	 *
	 */
	bool Application::addListener(ComponentId id, ComponentEventType eventType)
	{
		SYS_TX_T tx = platformCreateMessageTransaction();

		CommandAddListenerRequest request;
		request.header.id = FENSTER_PROTOCOL_ADD_LISTENER;
		request.id = id;
		request.target_thread = EventDispatcherTaskId;
		request.event_type = eventType;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandAddListenerRequest), tx);
		platformYieldTo(DelegateTaskId);

		size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandAddListenerResponse);
		uint8_t buffer[bufferSize];
		if(platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandAddListenerResponse*) SYS_MESSAGE_CONTENT(buffer);
			return response->status == FENSTER_PROTOCOL_SUCCESS;
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
		request.header.id = FENSTER_PROTOCOL_REGISTER_DESKTOP_CANVAS;
		request.canvas_id = c->getId();
		request.target_thread = EventDispatcherTaskId;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandRegisterDesktopCanvasRequest), tx);
		platformYieldTo(DelegateTaskId);

		// read response
		size_t buflen = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandRegisterDesktopCanvasResponse);
		uint8_t buf[buflen];

		bool success = false;
		if(platformReceiveMessage(buf, buflen, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
		{
			auto response = (CommandRegisterDesktopCanvasResponse*) SYS_MESSAGE_CONTENT(buf);
			success = (response->status == FENSTER_PROTOCOL_SUCCESS);
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
		request.header.id = FENSTER_PROTOCOL_GET_SCREEN_DIMENSION;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandGetScreenDimensionRequest), tx);
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

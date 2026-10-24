// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _GHOST_

#include "platform/platform.hpp"
#include "platform/ghost/ghost.hpp"
#include "server.hpp"
#include "components/cursor.hpp"
#include "events/event_processor.hpp"

#include <libfenster/input/key_info.hpp>
#include <libinput/keyboard/keyboard.hpp>
#include <libinput/mouse/mouse.hpp>
#include <libps2driver/ps2driver.hpp>

static g_fd keyboardIn = G_FD_NONE;
static g_fd mouseIn = G_FD_NONE;
static SYS_MUTEX_T inputInitializationLock = fenster::platformInitializeMutex(false);
static bool inputInitialized = false;

static bool inputReceiverAwaitInitialization()
{
	fenster::platformAcquireMutex(inputInitializationLock);
	const bool initialized = inputInitialized;
	fenster::platformReleaseMutex(inputInitializationLock);
	return initialized;
}

void inputReceiverInitialize()
{
	// Receivers may run immediately on another processor. Publish their file
	// descriptors before either enters a blocking read.
	fenster::platformAcquireMutex(inputInitializationLock);
	g_create_task_status keyStatus;
	g_create_task_status mouseStatus;
	g_tid keyEventThread = g_create_task_ds((void*) inputReceiverStartReceiveKeyEvents, nullptr, &keyStatus);
	g_tid mouseEventThread = g_create_task_ds((void*) inputReceiverStartReceiveMouseEvents, nullptr, &mouseStatus);
	inputInitialized = keyStatus == G_CREATE_TASK_STATUS_SUCCESSFUL &&
	                   mouseStatus == G_CREATE_TASK_STATUS_SUCCESSFUL &&
	                   ps2DriverInitialize(&keyboardIn, &mouseIn, keyEventThread, mouseEventThread);
	fenster::platformReleaseMutex(inputInitializationLock);

	if(!inputInitialized)
		fenster::platformLog("failed to initialize PS/2 input; input receiver tasks stopped");
}

void inputReceiverStartReceiveKeyEvents()
{
	if(!inputReceiverAwaitInitialization())
		return;

	fenster::platformRegisterTaskIdentifier("windowserver/key-receiver");

	fensterserver::EventProcessor* event_queue = fensterserver::Server::instance()->eventProcessor;

	while(true)
	{
		g_key_info ghostKey = g_keyboard::readKey(keyboardIn);

		fenster::KeyInfo key;
		key.pressed = ghostKey.pressed;
		key.ctrl = ghostKey.ctrl;
		key.alt = ghostKey.alt;
		key.shift = ghostKey.shift;
		key.scancode = ghostKey.scancode;
		key.key = ghostKey.key;

		if(key.ctrl && key.key == "KEY_Q" && key.pressed)
		{
			fensterserver::Server::setDebug(!fensterserver::Server::isDebug());
			continue;
		}

		event_queue->bufferKeyEvent(key);

		fensterserver::Server::instance()->requestUpdateImmediately();
	}
}

void inputReceiverStartReceiveMouseEvents()
{
	if(!inputReceiverAwaitInitialization())
		return;

	fenster::platformRegisterTaskIdentifier("windowserver/mouse-receiver");

	fensterserver::Server* instance = fensterserver::Server::instance();
	fenster::Dimension resolution = instance->videoOutput->getResolution();

	while(true)
	{
		g_mouse_info info = g_mouse::readMouse(mouseIn);
		const auto arrivalMillis = fenster::platformMillis();

		fenster::platformAcquireMutex(fensterserver::Cursor::inputLock);
		fensterserver::Cursor::nextPosition.x += info.x;
		fensterserver::Cursor::nextPosition.y += info.y;

		if(fensterserver::Cursor::nextPosition.x < 0)
		{
			fensterserver::Cursor::nextPosition.x = 0;
		}
		if(fensterserver::Cursor::nextPosition.x > resolution.width - 2)
		{
			fensterserver::Cursor::nextPosition.x = resolution.width - 2;
		}
		if(fensterserver::Cursor::nextPosition.y < 0)
		{
			fensterserver::Cursor::nextPosition.y = 0;
		}
		if(fensterserver::Cursor::nextPosition.y > resolution.height - 2)
		{
			fensterserver::Cursor::nextPosition.y = resolution.height - 2;
		}

		fensterserver::Cursor::nextPressedButtons = fenster::MouseButton::None;
		if(info.button1)
		{
			fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button1;
		}
		if(info.button2)
		{
			fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button2;
		}
		if(info.button3)
		{
			fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button3;
		}

		fenster::platformReleaseMutex(fensterserver::Cursor::inputLock);

		instance->eventProcessor->bufferMouseState(info.scroll, arrivalMillis);
		instance->requestPointerUpdate();
	}
}

#endif

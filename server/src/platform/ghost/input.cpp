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

static g_fd keyboardIn;
static g_fd mouseIn;

void inputReceiverInitialize()
{
	g_tid keyEventThread = g_create_task((void*) inputReceiverStartReceiveKeyEvents);
	g_tid mouseEventThread = g_create_task((void*) inputReceiverStartReceiveMouseEvents);
	ps2DriverInitialize(&keyboardIn, &mouseIn, keyEventThread, mouseEventThread);
}

void inputReceiverStartReceiveKeyEvents()
{
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
	fenster::platformRegisterTaskIdentifier("windowserver/mouse-receiver");

	fensterserver::Server* instance = fensterserver::Server::instance();
	fenster::Dimension resolution = instance->videoOutput->getResolution();

	while(true)
	{
		g_mouse_info info = g_mouse::readMouse(mouseIn);

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

		fensterserver::Cursor::nextPressedButtons = FENSTER_MOUSE_BUTTON_NONE;
		if(info.button1)
		{
			fensterserver::Cursor::nextPressedButtons |= FENSTER_MOUSE_BUTTON_1;
		}
		if(info.button2)
		{
			fensterserver::Cursor::nextPressedButtons |= FENSTER_MOUSE_BUTTON_2;
		}
		if(info.button3)
		{
			fensterserver::Cursor::nextPressedButtons |= FENSTER_MOUSE_BUTTON_3;
		}

		fensterserver::Cursor::nextScroll += info.scroll;

		fensterserver::Server::instance()->requestUpdateImmediately();
	}
}

#endif

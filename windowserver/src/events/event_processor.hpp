// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_EVENTS_EVENTPROCESSOR__
#define __WINDOWSERVER_EVENTS_EVENTPROCESSOR__

#include <deque>
#include "platform/platform.hpp"

#define DEFAULT_MULTICLICK_TIMESPAN 250

/**
 * The event queue is used to store any incoming events for
 * later processing.
 */
class event_processor_t
{
  public:
	uint32_t multiclickTimespan;

	event_processor_t();

	std::deque<g_key_info> key_info_buffer;
	SYS_MUTEX_T key_info_buffer_lock = platformInitializeMutex(false);
	void bufferKeyEvent(g_key_info keyInfo);

	void process();

	void translateKeyEvent(g_key_info& info);
	void processKeyState();
	void processMouseState();
};

#endif

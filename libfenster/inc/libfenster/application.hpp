// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_UI
#define LIBFENSTER_UI


#include "interface.hpp"
#include "metrics/dimension.hpp"


namespace fenster
{
	class Listener;
	class Canvas;

	enum class ApplicationOpenStatus: int {
		Success = 0,
		CommunicationFailed = 1,
		Error = 2,
		Existing = 3,
	};

	struct ApplicationEventDispatchData
	{
		Listener* listener;
		uint8_t* data;
		uint32_t length;
	};

	/**
	 * ID of the thread that the window server creates when
	 * initializing the UI communication.
	 */
	extern SYS_TID_T DelegateTaskId;

	/**
	 * ID of the event dispatcher thread that is continuously waiting
	 * for events from the window manager to fire the respective listener
	 * that was attached.
	 */
	extern SYS_TID_T EventDispatcherTaskId;

	class Application
	{
		static void eventDispatchThread();

	public:
		static ApplicationOpenStatus open();
		static bool registerDesktopCanvas(Canvas* c);
		static bool getScreenDimension(Dimension& out);

		static bool addListener(ComponentId id, ComponentEventType eventType);
	};
}

#endif

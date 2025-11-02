// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_DESKTOPCANVASLISTENER
#define LIBFENSTER_LISTENER_DESKTOPCANVASLISTENER

#include "listener.hpp"
#include <bits/std_function.h>
#include <utility>

namespace fenster
{
	typedef std::function<void(WindowsEvent*)> DesktopCanvasListenerFunc;

	class DesktopCanvasListener : public Listener
	{
	public:
		void process(ComponentEventHeader* header) override
		{
			handleEvent((WindowsEvent*) header);
		}

		virtual void handleEvent(WindowsEvent* event) = 0;
	};

	class DesktopCanvasListenerDispatcher : public DesktopCanvasListener
	{
		DesktopCanvasListenerFunc func;

	public:
		explicit DesktopCanvasListenerDispatcher(DesktopCanvasListenerFunc func):
			func(std::move(func))
		{
		}

		void handleEvent(WindowsEvent* event) override
		{
			func(event);
		}
	};
}

#endif

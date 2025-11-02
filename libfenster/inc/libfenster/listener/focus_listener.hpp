// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_FOCUSLISTENER
#define LIBFENSTER_LISTENER_FOCUSLISTENER

#include "listener.hpp"
#include "../interface.hpp"
#include <bits/std_function.h>

namespace fenster
{
	class Component;

	typedef std::function<void(bool)> FocusListenerFunc;

	class FocusListener : public Listener
	{
	public:
		void process(ComponentEventHeader* header) override
		{
			auto event = (ComponentFocusEvent*) header;
			handleFocusChanged(event->now_focused);
		}

		virtual void handleFocusChanged(bool nowFocused) = 0;
	};

	class FocusListenerDispatcher : public FocusListener
	{
		FocusListenerFunc func;

	public:
		explicit FocusListenerDispatcher(FocusListenerFunc func):
			func(std::move(func))
		{
		}

		void handleFocusChanged(bool nowFocused) override
		{
			func(nowFocused);
		}
	};
}

#endif

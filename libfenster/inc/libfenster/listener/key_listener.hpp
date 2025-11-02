// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_KEYLISTENER
#define LIBFENSTER_LISTENER_KEYLISTENER

#include "listener.hpp"
#include "libfenster/interface.hpp"
#include "libfenster/input/key_info.hpp"
#include <bits/std_function.h>

namespace fenster
{
	struct KeyEvent
	{
		KeyInfoBasic info;
	};

	typedef std::function<void(KeyEvent&)> KeyListenerFunc;

	class KeyListener : public Listener
	{
	public:
		void process(ComponentEventHeader* header) override
		{
			auto event = (ComponentKeyEvent*) header;

			KeyEvent e;
			e.info = event->key_info;
			handleKeyEvent(e);
		}

		virtual void handleKeyEvent(KeyEvent& e) = 0;
	};

	class KeyListenerDispatcher : public KeyListener
	{
		KeyListenerFunc func;

	public:
		explicit KeyListenerDispatcher(KeyListenerFunc func):
			func(std::move(func))
		{
		}

		void handleKeyEvent(KeyEvent& e) override
		{
			func(e);
		}
	};
}

#endif

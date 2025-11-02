// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_ACTIONLISTENER
#define LIBWINDOW_ACTIONLISTENER

#include "listener.hpp"
#include <utility>
#include <bits/std_function.h>

namespace fenster
{
	class Component;

	typedef std::function<void()> ActionListenerFunc;

	class ActionListener : public Listener
	{
	public:
		void process(ComponentEventHeader* header) override
		{
			handleAction();
		}

		virtual void handleAction() = 0;
	};

	class ActionListenerDispatcher : public ActionListener
	{
		ActionListenerFunc func;

	public:
		explicit ActionListenerDispatcher(ActionListenerFunc func): func(std::move(func))
		{
		}

		void handleAction() override
		{
			func();
		}
	};
}

#endif

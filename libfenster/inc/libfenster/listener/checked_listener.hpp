// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_CHECKEDLISTENER
#define LIBFENSTER_LISTENER_CHECKEDLISTENER

#include "libfenster/listener/listener.hpp"
#include <functional>
#include <utility>
#include <bits/std_function.h>

namespace fenster
{
	typedef std::function<void(bool)> CheckedListenerFunc;

	class CheckedListener : public Listener
	{
	public:
		~CheckedListener() override = default;

		void process(ComponentEventHeader* header) override
		{
			handleCheckedEvent((ComponentCheckedEvent*) header);
		}

		virtual void handleCheckedEvent(ComponentCheckedEvent* e) = 0;
	};

	class CheckedListenerDispatcher : public CheckedListener
	{
		CheckedListenerFunc func;

	public:
		explicit CheckedListenerDispatcher(CheckedListenerFunc func) :
			func(std::move(func))
		{
		};

		void handleCheckedEvent(ComponentCheckedEvent* e) override
		{
			func(e->checked);
		}
	};
}

#endif

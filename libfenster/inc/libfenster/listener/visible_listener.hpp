// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_VISIBLELISTENER
#define LIBFENSTER_LISTENER_VISIBLELISTENER

#include "listener.hpp"
#include <utility>
#include <bits/std_function.h>

namespace fenster
{
	class Component;

	typedef std::function<void(bool)> VisibleListenerFunc;

	class VisibleListener : public Listener
	{
	public:
		void process(ComponentEventHeader* header) override
		{
			auto e = (ComponentVisibleEvent*) header;
			handleVisibleChanged(e->visible);
		}

		virtual void handleVisibleChanged(bool visible) = 0;
	};

	class VisibleListenerDispatcher : public VisibleListener
	{
		VisibleListenerFunc func;

	public:
		explicit VisibleListenerDispatcher(VisibleListenerFunc func):
			func(std::move(func))
		{
		}

		void handleVisibleChanged(bool visible) override
		{
			func(visible);
		}
	};
}


#endif

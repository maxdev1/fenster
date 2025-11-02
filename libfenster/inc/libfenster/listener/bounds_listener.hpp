// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_BOUNDSLISTENER
#define LIBFENSTER_LISTENER_BOUNDSLISTENER

#include "listener.hpp"
#include <utility>
#include <bits/std_function.h>

namespace fenster
{
	class Component;

	typedef std::function<void(Rectangle)> BoundsListenerFunc;

	class BoundsListener : public Listener
	{
	public:
		void process(ComponentEventHeader* header) override
		{
			auto boundsEvent = (ComponentBoundsEvent*) header;
			handleBoundsChanged(boundsEvent->bounds);
		}

		virtual void handleBoundsChanged(Rectangle bounds) = 0;
	};

	class BoundsListenerDispatcher : public BoundsListener
	{
		BoundsListenerFunc func;

	public:
		explicit BoundsListenerDispatcher(BoundsListenerFunc func):
			func(std::move(func))
		{
		}

		void handleBoundsChanged(Rectangle bounds) override
		{
			func(bounds);
		}
	};
}

#endif

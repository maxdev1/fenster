// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_RELATIVEMOUSELISTENER
#define LIBFENSTER_LISTENER_RELATIVEMOUSELISTENER

#include "listener.hpp"
#include "libfenster/interface.hpp"
#include <functional>
#include <utility>

namespace fenster
{
	using RelativeMouseListenerFunc = std::function<void(ComponentRelativeMouseEvent*)>;
	class RelativeMouseListenerDispatcher : public Listener
	{
		RelativeMouseListenerFunc func;
	public:
		explicit RelativeMouseListenerDispatcher(RelativeMouseListenerFunc func) : func(std::move(func)) {}
		void process(ComponentEventHeader* event) override
		{
			func(reinterpret_cast<ComponentRelativeMouseEvent*>(event));
		}
	};

	using MouseGrabListenerFunc = std::function<void(bool)>;
	class MouseGrabListenerDispatcher : public Listener
	{
		MouseGrabListenerFunc func;
	public:
		explicit MouseGrabListenerDispatcher(MouseGrabListenerFunc func) : func(std::move(func)) {}
		void process(ComponentEventHeader* event) override
		{
			func(reinterpret_cast<ComponentMouseGrabEvent*>(event)->grabbed);
		}
	};
}

#endif

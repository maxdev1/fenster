// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_MOUSELISTENER
#define LIBWINDOW_MOUSELISTENER

#include "libwindow/listener/listener.hpp"
#include <functional>
#include <utility>
#include <bits/std_function.h>

namespace fenster
{
	typedef std::function<void(ComponentMouseEvent*)> MouseListenerFunc;

	class MouseListener : public Listener
	{
	public:
		~MouseListener() override = default;

		void process(ComponentEventHeader* header) override
		{
			handleMouseEvent((ComponentMouseEvent*) header);
		}

		virtual void handleMouseEvent(ComponentMouseEvent* e) = 0;
	};

	class MouseListenerDispatcher : public MouseListener
	{
		MouseListenerFunc func;

	public:
		explicit MouseListenerDispatcher(MouseListenerFunc func) :
			func(std::move(func))
		{
		};

		void handleMouseEvent(ComponentMouseEvent* e) override
		{
			func(e);
		}
	};
}

#endif

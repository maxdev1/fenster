// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_CANVASBUFFERLISTENER
#define LIBWINDOW_CANVASBUFFERLISTENER

#include <bits/std_function.h>
#include <utility>

namespace fenster
{
	typedef std::function<void()> CanvasBufferListenerFunc;

	class CanvasBufferListener
	{
	public:
		virtual ~CanvasBufferListener() = default;

		virtual void handleBufferChanged() = 0;
	};

	class CanvasBufferListenerDispatcher : public CanvasBufferListener
	{
		CanvasBufferListenerFunc func;

	public:
		explicit CanvasBufferListenerDispatcher(CanvasBufferListenerFunc func):
			func(std::move(func))
		{
		}

		void handleBufferChanged() override
		{
			func();
		}
	};
}

#endif

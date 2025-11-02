// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_CANVASBUFFERLISTENERINTERNAL
#define LIBWINDOW_CANVASBUFFERLISTENERINTERNAL

#include "listener.hpp"

namespace fenster
{
	class Component;
	class Canvas;

	/**
	 * Listener that a canvas registers on itself to react when a new buffer is ready.
	 */
	class CanvasBufferListenerInternal : public Listener
	{
	public:
		Canvas* canvas;

		explicit CanvasBufferListenerInternal(Canvas* canvas) :
			canvas(canvas)
		{
		}

		void process(ComponentEventHeader* header) override;
	};
}

#endif

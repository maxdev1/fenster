// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_CANVAS
#define LIBFENSTER_CANVAS

#include "libfenster/component.hpp"
#include "libfenster/focusable_component.hpp"
#include "libfenster/listener/canvas_buffer_listener.hpp"
#include "libfenster/listener/desktop_canvas_listener.hpp"
#include "libfenster/listener/canvas_buffer_listener_internal.hpp"

#include <cstdint>
#include <utility>
#include <cairo/cairo.h>


namespace fenster
{
	struct CanvasBufferInfo
	{
		uint8_t* buffer = nullptr;
		uint16_t width = 0;
		uint16_t height = 0;

		cairo_surface_t* surface = nullptr;
		cairo_t* context = nullptr;
		int contextRefCount = 0;
	};

	/**
	 * A canvas is a simple component that offers a buffer to the creator for arbitrary painting. This buffer automatically
	 * resizes when the bounds of the component change, so it is crucial to always keep track of which buffer to paint to.
	 */
	class Canvas : virtual public Component, virtual public FocusableComponent
	{
	protected:
		CanvasBufferInfo currentBuffer{};
		SYS_MUTEX_T currentBufferLock = platformInitializeMutex(false);

		/**
		 * Listener only for user purpose, so a client gets an event once the
		 * buffer was changed.
		 */
		CanvasBufferListener* bufferListener = nullptr;

		/**
		 * Template method that implementing classes can use to create their own
		 * canvas and automatically register all required listeners.
		 */
		template<typename COMPONENT_TYPE>
		static COMPONENT_TYPE* createCanvasComponent()
		{
			auto instance = createComponent<COMPONENT_TYPE, FENSTER_COMPONENT_TYPE_CANVAS>();
			if(instance)
				instance->addListener(FENSTER_COMPONENT_EVENT_TYPE_CANVAS_NEW_BUFFER,
				                      new CanvasBufferListenerInternal(instance));
			return instance;
		}

	public:
		explicit Canvas(ComponentId id) :
			Component(id), FocusableComponent(id)
		{
		}

		static Canvas* create();

		void acknowledgeNewBuffer(uintptr_t address, uint16_t width, uint16_t height);

		void blit(const Rectangle& rect);

		/**
		 * Acquires the current buffer and lends it to the user for painting. The buffer must be released afterwards.
		 */
		cairo_t* acquireGraphics();

		/**
		 * Releases the current buffer.
		 */
		void releaseGraphics();

		void setBufferListener(CanvasBufferListener* l)
		{
			bufferListener = l;
		}

		void setBufferListener(CanvasBufferListenerFunc func)
		{
			bufferListener = new CanvasBufferListenerDispatcher(std::move(func));
		}

		void setDesktopListener(DesktopCanvasListener* l)
		{
			this->addListener(FENSTER_COMPONENT_EVENT_TYPE_WINDOWS, l);
		}

		void setDesktopListener(DesktopCanvasListenerFunc func)
		{
			this->addListener(FENSTER_COMPONENT_EVENT_TYPE_WINDOWS,
			                  new DesktopCanvasListenerDispatcher(std::move(func)));
		}
	};
}

#endif

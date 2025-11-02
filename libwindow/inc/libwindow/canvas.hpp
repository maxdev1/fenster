// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_CANVAS
#define LIBWINDOW_CANVAS

#include "libwindow/component.hpp"
#include "libwindow/focusable_component.hpp"
#include "libwindow/listener/canvas_buffer_listener.hpp"
#include "libwindow/listener/desktop_canvas_listener.hpp"
#include "libwindow/listener/canvas_buffer_listener_internal.hpp"

#include <cstdint>
#include <utility>
#include <cairo/cairo.h>


struct g_canvas_buffer_info
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
class g_canvas : virtual public g_component, virtual public g_focusable_component
{
protected:
    g_canvas_buffer_info currentBuffer{};
    SYS_MUTEX_T currentBufferLock = platformInitializeMutex(false);

    /**
     * Listener only for user purpose, so a client gets an event once the
     * buffer was changed.
     */
    g_canvas_buffer_listener* bufferListener = nullptr;

    /**
     * Template method that implementing classes can use to create their own
     * canvas and automatically register all required listeners.
     */
    template <typename COMPONENT_TYPE>
    static COMPONENT_TYPE* createCanvasComponent()
    {
        auto instance = createComponent<COMPONENT_TYPE, G_UI_COMPONENT_TYPE_CANVAS>();
        if (instance)
            instance->addListener(G_UI_COMPONENT_EVENT_TYPE_CANVAS_NEW_BUFFER,
                                  new g_canvas_buffer_listener_internal(instance));
        return instance;
    }

public:
    explicit g_canvas(g_ui_component_id id) :
        g_component(id), g_focusable_component(id)
    {
    }

    static g_canvas* create();

    void acknowledgeNewBuffer(uintptr_t address, uint16_t width, uint16_t height);

    void blit(const g_rectangle& rect);

    /**
     * Acquires the current buffer and lends it to the user for painting. The buffer must be released afterwards.
     */
    cairo_t* acquireGraphics();

    /**
     * Releases the current buffer.
     */
    void releaseGraphics();

    void setBufferListener(g_canvas_buffer_listener* l)
    {
        bufferListener = l;
    }

    void setBufferListener(g_canvas_buffer_listener_func func)
    {
        bufferListener = new g_canvas_buffer_listener_dispatcher(std::move(func));
    }

    void setDesktopListener(g_desktop_canvas_listener* l)
    {
        this->addListener(G_UI_COMPONENT_EVENT_TYPE_WINDOWS, l);
    }

    void setDesktopListener(g_desktop_canvas_listener_func func)
    {
        this->addListener(G_UI_COMPONENT_EVENT_TYPE_WINDOWS, new g_desktop_canvas_listener_dispatcher(std::move(func)));
    }
};

#endif

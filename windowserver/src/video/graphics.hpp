// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_VIDEO_GRAPHICS
#define FENSTER_SERVER_VIDEO_GRAPHICS

#include <cairo/cairo.h>
#include <libwindow/metrics/dimension.hpp>
#include <libwindow/metrics/rectangle.hpp>
#include "platform/platform.hpp"

namespace fensterserver
{
    /**
     * The graphics class is a utility that internally holds a cairo surface and has
     * the ability to resize it when required.
     */
    class Graphics
    {
        cairo_t* context = nullptr;
        cairo_surface_t* surface = nullptr;
        SYS_MUTEX_T lock = fenster::platformInitializeMutex(true);

        int contextRefCount = 0;
        int averageFactor = 10;

    public:
        int width;
        int height;

        explicit Graphics(uint16_t width = 0, uint16_t height = 0);

        void resize(int width, int height, bool averaged = true);

        cairo_surface_t* getSurface() const
        {
            return surface;
        }

        void setAverageFactor(int factor)
        {
            this->averageFactor = factor;
        }

        cairo_t* acquireContext();

        void releaseContext();

        void blitTo(Graphics* target, const fenster::Rectangle& clip, const fenster::Point& position);
    };
}

#endif

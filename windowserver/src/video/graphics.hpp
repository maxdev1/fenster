// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_VIDEO_GRAPHICS__
#define __WINDOWSERVER_VIDEO_GRAPHICS__

#include <cairo/cairo.h>
#include <libwindow/metrics/dimension.hpp>
#include <libwindow/metrics/rectangle.hpp>
#include "platform/platform.hpp"

/**
 * The graphics class is a utility that internally holds a cairo surface and has
 * the ability to resize it when required.
 */
class graphics_t
{
    cairo_t* context = nullptr;
    cairo_surface_t* surface = nullptr;
    SYS_MUTEX_T lock = platformInitializeMutex(true);

    int contextRefCount = 0;
    int averageFactor = 10;

public:
    int width;
    int height;

    explicit graphics_t(uint16_t width = 0, uint16_t height = 0);

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

    void blitTo(graphics_t* target, const g_rectangle& clip, const g_point& position);
};

#endif

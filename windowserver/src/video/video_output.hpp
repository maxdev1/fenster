// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_VIDEOOUTPUT__
#define __WINDOWSERVER_VIDEOOUTPUT__

#include <libwindow/color_argb.hpp>
#include <libwindow/metrics/dimension.hpp>
#include <libwindow/metrics/rectangle.hpp>

/**
 * The video output is the abstract interface that the window server uses to
 * initialize a video mode and put output on the screen.
 */
class g_video_output
{
public:
    virtual ~g_video_output() = default;

    /**
     * Initializes the video mode implementation.
     *
     * @return whether initialization was successful
     */
    virtual bool initialize() = 0;

    /**
     * Writes the invalid rectangle within the source image to the screen.
     *
     * @param invalid
     * 		rectangle that is invalid
     * @param sourceSize
     * 		absolute size of the source
     * @param source
     * 		source buffer
     */
    virtual void blit(g_rectangle invalid, g_rectangle sourceSize, g_color_argb* source) = 0;

    /**
     * Returns the initialized resolution.
     */
    virtual g_dimension getResolution() = 0;
};

#endif

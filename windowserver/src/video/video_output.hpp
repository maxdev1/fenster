// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_VIDEO_VIDEOOUTPUT
#define FENSTER_SERVER_VIDEO_VIDEOOUTPUT

#include <libwindow/color_argb.hpp>
#include <libwindow/metrics/dimension.hpp>
#include <libwindow/metrics/rectangle.hpp>

namespace fensterserver
{
    /**
     * The video output is the abstract interface that the window server uses to
     * initialize a video mode and put output on the screen.
     */
    class VideoOutput
    {
    public:
        virtual ~VideoOutput() = default;

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
        virtual void blit(fenster::Rectangle invalid, fenster::Rectangle sourceSize, fenster::ColorArgb* source) = 0;

        /**
         * Returns the initialized resolution.
         */
        virtual fenster::Dimension getResolution() = 0;
    };
}

#endif

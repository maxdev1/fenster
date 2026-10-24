// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_VIDEO_CURSOROVERLAY
#define FENSTER_SERVER_VIDEO_CURSOROVERLAY

#include "video/graphics.hpp"
#include "video/video_output.hpp"
#include <vector>

namespace fensterserver
{
    /**
     * Presents cursor motion independently of component updates and painting.
     * The presentation buffer contains the last committed scene and the cursor;
     * a saved tile lets either presenter remove the cursor before changing it.
     */
    class CursorOverlay
    {
        VideoOutput* output;
        fenster::Rectangle bounds;
        Graphics presentation;
        SYS_MUTEX_T lock = fenster::platformInitializeMutex(false);
        fenster::Rectangle cursorArea;
        std::vector<fenster::ColorArgb> savedPixels;

        void restoreCursor();
        void saveAndPaintCursor();
        void flush(const fenster::Rectangle& area);
        void flushChanges(const fenster::Rectangle& damage, const fenster::Rectangle& oldCursor);

    public:
        CursorOverlay(VideoOutput* output, const fenster::Rectangle& bounds);

        void present();
        void commit(Graphics* scene, const fenster::Rectangle& damage);
    };
}

#endif

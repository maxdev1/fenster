// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_PLATFORM_PLATFORM
#define FENSTER_SERVER_PLATFORM_PLATFORM

#include <video/video_output.hpp>

#include <libfenster/platform/platform.hpp>
#include <libfenster/input/key_info.hpp>

namespace fensterserver
{
    /**
     * Starts whatever is necessary on the system to receive mouse and keyboard input.
     */
    void platformStartInput();

    /**
     * Creates an instance of the video output class. After creation, `initialize()`
     * will be executed on this object.
     */
    VideoOutput* platformCreateVideoOutput();

    /**
     * Loads all cursors from the platforms cursor configuration and calls
     * Cursor::load on every cursor to be loaded.
     */
    void platformLoadCursors();

    /**
     * Initializes the keyboard layout to the specified locale.
     */
    bool platformInitializeKeyboardLayout(std::string locale);

    /**
     * Converts the key information into a character.
     */
    char platformCharForKey(fenster::KeyInfo key);
}

#endif

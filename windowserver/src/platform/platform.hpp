// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __PLATFORM__
#define __PLATFORM__

#include <video/video_output.hpp>

#include <libwindow/platform/platform.hpp>
#include <libwindow/input/key_info.hpp>
#include <stdint.h>

/**
 * Starts whatever is necessary on the system to receive mouse and keyboard input.
 */
void platformStartInput();

/**
 * Creates an instance of the video output class. After creation, `initialize()`
 * will be executed on this object.
 */
g_video_output* platformCreateVideoOutput();

/**
 * Loads all cursors from the platforms cursor configuration and calls
 * cursor_t::load on every cursor to be loaded.
 */
void platformLoadCursors();

/**
 * Initializes the keyboard layout to the specified locale.
 */
bool platformInitializeKeyboardLayout(std::string locale);

/**
 * Converts the key information into a character.
 */
char platformCharForKey(key_info_t key);

#endif

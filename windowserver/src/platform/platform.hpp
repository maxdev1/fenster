// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __PLATFORM__
#define __PLATFORM__

#include <libwindow/platform/platform.hpp>

/**
 * Platform specific type definitions
 */
#include <stdint.h>
#include <video/video_output.hpp>

/**
 * Starts whatever is necessary on the system to receive mouse and keyboard input.
 */
void platformStartInput();

/**
 * Creates an instance of the video output class. After creation, `initialize()`
 * will be executed on this object.
 */
g_video_output* platformCreateVideoOutput();

void platformLoadCursors();

bool platformInitializeKeyboardLayout(std::string layout);

#endif

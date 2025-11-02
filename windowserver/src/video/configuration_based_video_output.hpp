// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __CONFIGURATION_BASED_VIDEO_OUTPUT__
#define __CONFIGURATION_BASED_VIDEO_OUTPUT__

#include "video_output.hpp"

/**
 * A configuration based video output reads a configuration file first to
 * determine what video settings shall be used.
 */
class g_configuration_based_video_output : public g_video_output
{
  public:
    virtual ~g_configuration_based_video_output()
    {
    }

    virtual bool initialize();
    virtual bool initializeWithSettings(uint32_t width, uint32_t height, uint32_t bits) = 0;
    virtual void blit(g_rectangle invalid, g_rectangle sourceSize, g_color_argb* source) = 0;
};

#endif

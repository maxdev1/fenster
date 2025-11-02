// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_VIDEO_CONFIGURATIONBASEDVIDEOOUTPUT
#define FENSTER_SERVER_VIDEO_CONFIGURATIONBASEDVIDEOOUTPUT

#include "video_output.hpp"

namespace fensterserver
{
    /**
     * A configuration based video output reads a configuration file first to
     * determine what video settings shall be used.
     */
    class ConfigurationBasedVideoOutput : public VideoOutput
    {
    public:
        virtual ~ConfigurationBasedVideoOutput()
        {
        }

        virtual bool initialize();
        virtual bool initializeWithSettings(uint32_t width, uint32_t height, uint32_t bits) = 0;
        virtual void blit(fenster::Rectangle invalid, fenster::Rectangle sourceSize, fenster::ColorArgb* source) = 0;
    };
}

#endif

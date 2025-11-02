// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __GENERIC_VIDEO_OUTPUT__
#define __GENERIC_VIDEO_OUTPUT__

#include "video/configuration_based_video_output.hpp"
#include "platform/platform.hpp"
#include <libvideo/videodriver.hpp>

class GenericVideoOutput : public fensterserver::ConfigurationBasedVideoOutput
{
    g_video_mode_info videoModeInformation{};

    SYS_TID_T driverTid;
    g_device_id deviceId;

public:
    explicit GenericVideoOutput(SYS_TID_T driverTid, g_device_id deviceId): driverTid(driverTid), deviceId(deviceId)
    {
    }

    bool initializeWithSettings(uint32_t width, uint32_t height, uint32_t bits) override;
    void blit(fenster::Rectangle invalid, fenster::Rectangle sourceSize, fenster::ColorArgb* source) override;
    fenster::Dimension getResolution() override;
};

#endif

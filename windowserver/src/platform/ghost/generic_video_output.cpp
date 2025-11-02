// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _GHOST_

#include "generic_video_output.hpp"
#include <libvideo/videodriver.hpp>
#include <cstdio>
#include <immintrin.h>

bool GenericVideoOutput::initializeWithSettings(uint32_t width, uint32_t height, uint32_t bits)
{
	int tries = 3;
	while(!videoDriverSetMode(driverTid, deviceId, width, height, bits, videoModeInformation))
	{
		fenster::platformLog("failed to initialize generic video... retrying");
		if(tries-- == 0)
			return false;
		fenster::platformSleep(1000);
	}
	return true;
}

void GenericVideoOutput::blit(fenster::Rectangle invalid, fenster::Rectangle sourceSize, fenster::ColorArgb* source)
{
	uint16_t bpp = videoModeInformation.bpp;
	uint8_t* position = ((uint8_t*) videoModeInformation.lfb) + (invalid.y * videoModeInformation.bpsl);

	int right = invalid.x + invalid.width;
	int bottom = invalid.y + invalid.height;

	if(bpp == 32)
	{
		for(int y = invalid.y; y < bottom; y++)
		{
			auto sourceRow = &source[y * sourceSize.width];

			auto position4 = (uint32_t*) position;
			int x = invalid.x;
			for(; x < right - 4; x += 4)
			{
				__m128i data = _mm_loadu_si128((__m128i*) &sourceRow[x]);
				_mm_storeu_si128((__m128i*) &position4[x], data);
			}
			for(; x < right; x++)
			{
				position4[x] = sourceRow[x];
			}
			position += videoModeInformation.bpsl;
		}
	}
	else if(bpp == 24)
	{
		for(int y = invalid.y; y < bottom; y++)
		{
			for(int x = invalid.x; x < right; x++)
			{
				fenster::ColorArgb color = source[y * sourceSize.width + x];
				position[x * 3] = color & 0xFF;
				position[x * 3 + 1] = (color >> 8) & 0xFF;
				position[x * 3 + 2] = (color >> 16) & 0xFF;
			}
			position += videoModeInformation.bpsl;
		}
	}

	if(videoModeInformation.explicit_update)
	{
		videoDriverUpdate(driverTid, deviceId, invalid.x, invalid.y, invalid.width, invalid.height);
		g_yield();
	}
}


fenster::Dimension GenericVideoOutput::getResolution()
{
	return {videoModeInformation.resX, videoModeInformation.resY};
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __PLATFORM_MACOSSDLSIM__
#define __PLATFORM_MACOSSDLSIM__
#ifdef __APPLE__

#include <SDL2/SDL.h>
#include <string>

namespace fensterserver
{
    std::string platformKeyForSDLKey(const SDL_Keysym &ks);
    void platformServerThread();
}

#endif
#endif

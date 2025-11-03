// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __PLATFORM_WINDOWSMINGWSIM__
#define __PLATFORM_WINDOWSMINGWSIM__
#ifdef _WIN32

#include <string>

#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#define GET_X_LPARAM(lParam)            ((int)(short)LOWORD(lParam))
#define GET_Y_LPARAM(lParam)            ((int)(short)HIWORD(lParam))

namespace fensterserver
{
    std::string platformKeyForVKey(uint8_t scancode);
    void platformServerThread();
}

#endif
#endif

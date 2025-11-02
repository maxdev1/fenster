// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_COLORARGB
#define LIBWINDOW_COLORARGB

#include <cstdint>

#define ARGB(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | (b))
#define RGB(r, g, b) ARGB(0xFF, r, g, b)

#define ARGB_A_FROM(argb) ((argb >> 24) & 0xFF)
#define ARGB_R_FROM(argb) ((argb >> 16) & 0xFF)
#define ARGB_G_FROM(argb) ((argb >> 8) & 0xFF)
#define ARGB_B_FROM(argb) ((argb >> 0) & 0xFF)

#define ARGB_FA_FROM(argb) ((double) ((argb >> 24) & 0xFF) / 255)
#define ARGB_FR_FROM(argb) ((double) ((argb >> 16) & 0xFF) / 255)
#define ARGB_FG_FROM(argb) ((double) ((argb >> 8) & 0xFF) / 255)
#define ARGB_FB_FROM(argb) ((double) ((argb >> 0) & 0xFF) / 255)

#define G_COLOR_ARGB_TO_FPARAMS(argb) ARGB_FR_FROM(argb), ARGB_FG_FROM(argb), ARGB_FB_FROM(argb), ARGB_FA_FROM(argb)

typedef uint32_t g_color_argb;

#endif

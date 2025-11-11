// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_UTIL_CAIRO
#define FENSTER_SERVER_UTIL_CAIRO

#include <cairo/cairo.h>

void cairo_rounded_rectangle(cairo_t* cr, double x, double y, double width, double height, double radius);

#endif
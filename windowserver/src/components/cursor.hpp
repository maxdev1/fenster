// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_CURSOR__
#define __WINDOWSERVER_COMPONENTS_CURSOR__

#include "components/component.hpp"

#include <cairo/cairo.h>
#include <fstream>
#include <libwindow/metrics/point.hpp>

#define FALLBACK_CURSOR_SIZE 10

struct cursor_configuration
{
    std::string name;
    cairo_surface_t* surface;
    g_point hitpoint;
    g_dimension size;
};

class cursor_t
{
public:
    static g_point position;
    static g_point nextPosition;
    static g_mouse_button pressedButtons;
    static g_mouse_button nextPressedButtons;
    static int nextScroll;

    static g_ui_component_id pressedComponent;
    static g_ui_component_id draggedComponent;
    static g_ui_component_id hoveredComponent;
    static g_ui_component_id focusedComponent;

    static void paint(graphics_t* global);

    static g_rectangle getArea();

    static std::string get();
    static void set(std::string name);

    static bool load(std::string cursorPath);
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_CURSOR
#define FENSTER_SERVER_COMPONENTS_CURSOR

#include "components/component.hpp"

#include <cairo/cairo.h>
#include <fstream>
#include <libfenster/metrics/point.hpp>

#define FALLBACK_CURSOR_SIZE 10

namespace fensterserver
{
    struct CursorConfiguration
    {
        std::string name;
        cairo_surface_t* surface;
        fenster::Point hitpoint;
        fenster::Dimension size;
    };

    class Cursor
    {
    public:
        static fenster::Point position;
        static fenster::Point nextPosition;
        static fenster::MouseButton pressedButtons;
        static fenster::MouseButton nextPressedButtons;
        static int nextScroll;

        static fenster::ComponentId pressedComponent;
        static fenster::ComponentId draggedComponent;
        static fenster::ComponentId hoveredComponent;
        static fenster::ComponentId focusedComponent;

        static void paint(Graphics* global);

        static fenster::Rectangle getArea();

        static std::string get();
        static void set(std::string name);

        static bool load(std::string cursorPath);
    };
}

#endif

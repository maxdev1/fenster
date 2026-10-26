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

    /** Immutable image retained while another task changes the cursor configuration. */
    struct CursorImage
    {
        cairo_surface_t* surface = nullptr;
        fenster::Point hitpoint;
        fenster::Dimension size;
        bool visible = false;

        CursorImage() = default;
        CursorImage(const CursorImage&) = delete;
        CursorImage(CursorImage&& other) noexcept :
            surface(other.surface), hitpoint(other.hitpoint), size(other.size), visible(other.visible)
        {
            other.surface = nullptr;
        }
        ~CursorImage()
        {
            if(surface)
                cairo_surface_destroy(surface);
        }

        fenster::Rectangle getAreaAt(const fenster::Point& position) const;
        void paint(cairo_surface_t* target, const fenster::Point& position, const fenster::Rectangle& clip) const;
    };

    class Cursor
    {
    public:
        static SYS_MUTEX_T inputLock;
        static SYS_MUTEX_T configurationLock;
		static bool hidden;
		static void setHidden(bool hidden);
        static fenster::Point position;
        static fenster::Point nextPosition;
        static fenster::MouseButton pressedButtons;
        static fenster::MouseButton nextPressedButtons;

        static fenster::ComponentId pressedComponent;
        static fenster::ComponentId draggedComponent;
        static fenster::ComponentId hoveredComponent;
        static fenster::ComponentId focusedComponent;

        static void paint(Graphics* global, const fenster::Rectangle& clip);
        static void paintAt(Graphics* global, const fenster::Point& position, const fenster::Rectangle& clip);

        static fenster::Rectangle getArea();
        static fenster::Rectangle getAreaAt(const fenster::Point& position);
        static CursorImage snapshot();

        static std::string get();
        static void set(std::string name);

        static bool load(std::string cursorPath);
    };
}

#endif

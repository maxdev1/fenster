// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_PROPERTIES
#define LIBFENSTER_PROPERTIES

/**
 * Properties may have a different meaning for each component. They are
 * used to simplify configuring components from a client application.
 */

#define FENSTER_UI_PROPERTY_MOVABLE           1
#define FENSTER_UI_PROPERTY_RESIZABLE         2
#define FENSTER_UI_PROPERTY_SECURE			  3
#define FENSTER_UI_PROPERTY_ENABLED			  4
#define FENSTER_UI_PROPERTY_LAYOUT_MANAGER	  5
#define FENSTER_UI_PROPERTY_BACKGROUND        6
#define FENSTER_UI_PROPERTY_COLOR             7
#define FENSTER_UI_PROPERTY_ALIGNMENT         8
#define FENSTER_UI_PROPERTY_FOCUSED           9
#define FENSTER_UI_PROPERTY_VISIBLE           10
#define FENSTER_UI_PROPERTY_FOCUSABLE         11
#define FENSTER_UI_PROPERTY_DISPATCHES_FOCUS  12
#define FENSTER_UI_PROPERTY_IMAGE_SOURCE      14
#define FENSTER_UI_PROPERTY_STYLE             15
#define FENSTER_UI_PROPERTY_TITLE             16
#define FENSTER_UI_PROPERTY_CHECKED           17

/**
 * Properties for layout managers that also might have a different meaning
 * per layout manager.
 */
#define FENSTER_UI_PROPERTY_LAYOUT_HORIZONTAL		20
#define FENSTER_UI_PROPERTY_LAYOUT_SPACE			21
#define FENSTER_UI_PROPERTY_LAYOUT_PADDING			22
#define FENSTER_UI_PROPERTY_LAYOUT_COMPONENT_INFO	23

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_COMPONENT_REGISTRY
#define LIBWINDOW_COMPONENT_REGISTRY

#include "libwindow/interface.hpp"

// forward declaration
class g_component;

class g_component_registry
{
  public:
	static void add(g_component *component);
	static g_component *get(g_ui_component_id id);
};

#endif

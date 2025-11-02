// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libwindow/component.hpp"
#include "libwindow/bounding_component.hpp"

bool g_bounding_component::setBoundsListener(g_bounds_listener* new_listener)
{
	return self->addListener(G_UI_COMPONENT_EVENT_TYPE_BOUNDS, new_listener);
}

bool g_bounding_component::setBoundsListener(g_bounds_listener_func func)
{
	return self->addListener(G_UI_COMPONENT_EVENT_TYPE_BOUNDS, new g_bounds_listener_dispatcher(std::move(func)));
}

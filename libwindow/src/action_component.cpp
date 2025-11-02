// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/component.hpp"
#include "libwindow/action_component.hpp"

bool g_action_component::setActionListener(g_action_listener* new_listener)
{
	return self->addListener(G_UI_COMPONENT_EVENT_TYPE_ACTION, new_listener);
}

bool g_action_component::setActionListener(g_action_listener_func func)
{
	return self->addListener(G_UI_COMPONENT_EVENT_TYPE_ACTION, new g_action_listener_dispatcher(func));
}

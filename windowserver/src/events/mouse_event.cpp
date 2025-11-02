// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "events/mouse_event.hpp"
#include "components/component.hpp"

component_t* mouse_event_t::visit(component_t* component)
{
    return component->handleMouseEvent(*this);
}

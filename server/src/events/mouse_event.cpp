// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "events/mouse_event.hpp"
#include "components/component.hpp"

namespace fensterserver
{
	Component* MouseEvent::visit(Component* component)
	{
		return component->handleMouseEvent(*this);
	}
}

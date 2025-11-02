// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "events/key_event.hpp"
#include "components/component.hpp"

namespace fensterserver
{
	Component* KeyEvent::visit(Component* component)
	{
		return component->handleKeyEvent(*this);
	}
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/component.hpp"
#include "libwindow/action_component.hpp"

namespace fenster
{
	bool ActionComponent::setActionListener(ActionListener* new_listener)
	{
		return self->addListener(FENSTER_COMPONENT_EVENT_TYPE_ACTION, new_listener);
	}

	bool ActionComponent::setActionListener(ActionListenerFunc func)
	{
		return self->addListener(FENSTER_COMPONENT_EVENT_TYPE_ACTION, new ActionListenerDispatcher(func));
	}
}

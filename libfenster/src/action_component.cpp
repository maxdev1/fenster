// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/component.hpp"
#include "libfenster/action_component.hpp"

namespace fenster
{
	bool ActionComponent::setActionListener(ActionListener* new_listener)
	{
		return self->addListener(ComponentEventType::Action, new_listener);
	}

	bool ActionComponent::setActionListener(ActionListenerFunc func)
	{
		return self->addListener(ComponentEventType::Action, new ActionListenerDispatcher(func));
	}
}

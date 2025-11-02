// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libfenster/component.hpp"
#include "libfenster/bounding_component.hpp"

namespace fenster
{
	bool BoundingComponent::setBoundsListener(BoundsListener* new_listener)
	{
		return self->addListener(FENSTER_COMPONENT_EVENT_TYPE_BOUNDS, new_listener);
	}

	bool BoundingComponent::setBoundsListener(BoundsListenerFunc func)
	{
		return self->addListener(FENSTER_COMPONENT_EVENT_TYPE_BOUNDS, new BoundsListenerDispatcher(std::move(func)));
	}
}

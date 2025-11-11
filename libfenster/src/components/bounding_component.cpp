// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libfenster/components/component.hpp"
#include "libfenster/components/bounding_component.hpp"

namespace fenster
{
	bool BoundingComponent::setBoundsListener(BoundsListener* new_listener)
	{
		return self->addListener(ComponentEventType::Bounds, new_listener);
	}

	bool BoundingComponent::setBoundsListener(BoundsListenerFunc func)
	{
		return self->addListener(ComponentEventType::Bounds, new BoundsListenerDispatcher(std::move(func)));
	}
}

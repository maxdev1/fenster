// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBWINDOW_BOUNDINGCOMPONENT__
#define __LIBWINDOW_BOUNDINGCOMPONENT__

#include "listener/bounds_listener.hpp"

namespace fenster
{
	/**
	 * Component that is capable of receiving bounds events
	 */
	class BoundingComponent
	{
	protected:
		Component* self;

	public:
		explicit BoundingComponent(Component* self) : self(self)
		{
		}

		virtual ~BoundingComponent() = default;

		bool setBoundsListener(BoundsListener* l);
		bool setBoundsListener(BoundsListenerFunc func);
	};
}

#endif

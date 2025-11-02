// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_ACTIONCOMPONENT
#define LIBFENSTER_ACTIONCOMPONENT

#include "listener/action_listener.hpp"
#include "interface.hpp"
#include "component.hpp"

namespace fenster
{
	/**
	 * Component that is capable of receiving action events
	 */
	class ActionComponent : virtual public Component
	{
	protected:
		explicit ActionComponent(ComponentId id) :
			Component(id)
		{
		}

	public:
		~ActionComponent() override = default;

		bool setActionListener(ActionListener* l);
		bool setActionListener(ActionListenerFunc func);
	};
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_CHECKABLECOMPONENT
#define LIBFENSTER_CHECKABLECOMPONENT

#include <bits/std_function.h>

#include "interface.hpp"
#include "component.hpp"
#include "listener/checked_listener.hpp"

namespace fenster
{
	class CheckableComponent : virtual public Component
	{
	protected:
		explicit CheckableComponent(ComponentId id) :
			Component(id)
		{
		}

	public:
		~CheckableComponent() override = default;

		virtual bool setChecked(bool checked);
		virtual bool isChecked();

		virtual void addCheckedListener(CheckedListenerFunc callback);
	};
}

#endif

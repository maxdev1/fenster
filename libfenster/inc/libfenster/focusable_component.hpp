// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_FOCUSABLECOMPONENT
#define LIBFENSTER_FOCUSABLECOMPONENT

#include <bits/std_function.h>

#include "interface.hpp"
#include "component.hpp"

namespace fenster
{
	class FocusableComponent : virtual public Component
	{
	protected:
		explicit FocusableComponent(ComponentId id) :
			Component(id)
		{
		}

	public:
		~FocusableComponent() override = default;

		virtual bool setFocused(bool focused);
		virtual bool isFocused();

		virtual void addFocusListener(std::function<void(bool)> callback);
	};
}

#endif

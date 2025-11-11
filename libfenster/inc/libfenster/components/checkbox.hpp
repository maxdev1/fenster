// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_CHECKBOX
#define LIBFENSTER_CHECKBOX

#include "component.hpp"
#include "titled_component.hpp"
#include "checkable_component.hpp"

namespace fenster
{
	class Checkbox : virtual public Component, virtual public TitledComponent, virtual public CheckableComponent
	{
	public:
		explicit Checkbox(ComponentId id) : Component(id), TitledComponent(id), CheckableComponent(id)
		{
		}

		static Checkbox* create();

		void setEnabled(bool enabled);
		bool isEnabled();
	};
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_BUTTON
#define LIBFENSTER_BUTTON

#include "action_component.hpp"
#include "component.hpp"
#include "titled_component.hpp"

namespace fenster
{
	enum class ButtonVariant: uint32_t
	{
		Default = 0,
		Ghost = 1
	};

	class Button : virtual public Component, virtual public TitledComponent, virtual public ActionComponent
	{
	public:
		explicit Button(ComponentId id) :
			Component(id), TitledComponent(id), ActionComponent(id)
		{
		}

		static Button* create();

		void setEnabled(bool enabled);
		bool isEnabled();

		void setVariant(ButtonVariant variant);
		ButtonVariant getVariant();
	};
}

#endif

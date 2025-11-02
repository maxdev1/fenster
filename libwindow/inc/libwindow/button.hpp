// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_BUTTON
#define LIBWINDOW_BUTTON

#include "action_component.hpp"
#include "component.hpp"
#include "titled_component.hpp"

namespace fenster
{
	typedef uint8_t ButtonStyle;
#define FENSTER_BUTTON_STYLE_DEFAULT  0
#define FENSTER_BUTTON_STYLE_GHOST    1

	class Button : virtual public Component, virtual public TitledComponent, virtual public ActionComponent
	{
	public:
		explicit Button(ComponentId id) : Component(id), TitledComponent(id), ActionComponent(id)
		{
		}

		static Button* create();

		void setEnabled(bool enabled);
		bool isEnabled();

		void setStyle(ButtonStyle style);
		ButtonStyle getStyle();
	};
}

#endif

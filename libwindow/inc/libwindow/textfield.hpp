// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_TEXTFIELD
#define LIBWINDOW_TEXTFIELD

#include "component.hpp"
#include "titled_component.hpp"

namespace fenster
{
	class TextField : virtual public Component, virtual public TitledComponent
	{
	public:
		explicit TextField(ComponentId id) :
			Component(id), TitledComponent(id)
		{
		}

		static TextField* create();

		void setSecure(bool secure);
		bool isSecure();
	};
}

#endif

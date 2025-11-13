// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_TEXTBOX
#define LIBFENSTER_TEXTBOX

#include "component.hpp"
#include "titled_component.hpp"

namespace fenster
{
	class TextBox : virtual public Component, virtual public TitledComponent
	{
	public:
		explicit TextBox(ComponentId id) :
			Component(id), TitledComponent(id)
		{
		}

		static TextBox* create();

		void setSecure(bool secure);
		bool isSecure();

		void setMultiLine(bool multiline);
	};
}

#endif

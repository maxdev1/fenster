// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_WINDOW
#define LIBWINDOW_WINDOW

#include "component.hpp"
#include "focusable_component.hpp"
#include "titled_component.hpp"

namespace fenster
{
	class Window :
		virtual public Component,
		virtual public TitledComponent,
		virtual public FocusableComponent
	{
	public:
		explicit Window(ComponentId id):
			Component(id), TitledComponent(id), FocusableComponent(id)
		{
		}

		static Window* create();
		static Window* attach(ComponentId id);

		bool isResizable();
		void setResizable(bool resizable);

		bool onClose(std::function<void()> func);
	};
}

#endif

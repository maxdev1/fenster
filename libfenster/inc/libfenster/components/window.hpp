// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_WINDOW
#define LIBFENSTER_WINDOW

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
		void setShowInTaskbar(bool show) { setNumericProperty(ComponentProperty::ShowInTaskbar, show); }
		void setMinimizeButtonVisible(bool visible) { setNumericProperty(ComponentProperty::MinimizeButtonVisible, visible); }
		void setMaximizeButtonVisible(bool visible) { setNumericProperty(ComponentProperty::MaximizeButtonVisible, visible); }

		bool onClose(std::function<void()> func);
	};
}

#endif

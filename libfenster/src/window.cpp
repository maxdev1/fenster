// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <functional>
#include <utility>

#include "libfenster/properties.hpp"
#include "libfenster/window.hpp"

namespace fenster
{
	class WindowCloseDispatcher : public Listener
	{
		std::function<void()> func;

	public:
		explicit WindowCloseDispatcher(std::function<void()> func) :
			func(std::move(func))
		{
		}

		void process(ComponentEventHeader* header) override
		{
			func();
		}
	};

	Window* Window::create()
	{
		return createComponent<Window, ComponentType::Window>();
	}

	Window* Window::attach(ComponentId id)
	{
		return attachComponent<Window>(id);
	}

	bool Window::isResizable()
	{
		uint32_t value;
		getNumericProperty(ComponentProperty::Resizable, &value);
		return value;
	}

	void Window::setResizable(bool resizable)
	{
		setNumericProperty(ComponentProperty::Resizable, resizable);
	}

	bool Window::onClose(std::function<void()> func)
	{
		return addListener(ComponentEventType::Close, new WindowCloseDispatcher(std::move(func)));
	}
}

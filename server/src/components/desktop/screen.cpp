// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/desktop/screen.hpp"
#include "components/window.hpp"
#include "components/menu.hpp"

#include <cstring>

namespace fensterserver
{
	void Screen::addChild(Component* comp, ComponentChildReferenceType type)
	{
		Component::addChild(comp, type);

		if(comp->isWindow())
		{
			this->callForListeners(fenster::ComponentEventType::Window, [this, comp](EventListenerInfo& info)
			{
				auto window = dynamic_cast<Window*>(comp);
				sendWindowEvent(info.component_id, window, info.target_thread, true);
			});
		}
	}

	void Screen::removeChild(Component* comp)
	{
		if(comp->isWindow())
		{
			this->callForListeners(fenster::ComponentEventType::Window, [this, comp](EventListenerInfo& info)
			{
				sendWindowEvent(info.component_id, dynamic_cast<Window*>(comp), info.target_thread,
				                false);
			});
		}

		markDirty(comp->getBounds());
		Component::removeChild(comp);
	}

	void Screen::sendWindowEvent(fenster::ComponentId observerId, Window* window, SYS_TID_T observerThread,
	                             bool present)
	{
		fenster::WindowsEvent windowEvent;
		windowEvent.header.type = fenster::ComponentEventType::Window;
		windowEvent.header.component_id = observerId;
		windowEvent.window_id = window->id;
		windowEvent.present = present;
		fenster::platformSendMessage(observerThread, &windowEvent, sizeof(fenster::WindowsEvent), SYS_TX_NONE);
	}

	void Screen::markDirty(fenster::Rectangle rect)
	{
		fenster::platformAcquireMutex(lock);

		if(invalid.width == 0 && invalid.height == 0)
		{
			invalid = rect;
		}
		else
		{
			invalid.extend(rect.getStart());
			invalid.extend(rect.getEnd());
		}

		// Fix invalid area
		if(invalid.x < 0)
		{
			invalid.width += invalid.x;
			invalid.x = 0;
		}
		if(invalid.y < 0)
		{
			invalid.height += invalid.y;
			invalid.y = 0;
		}
		if(invalid.x + invalid.width > getBounds().width)
		{
			invalid.width = getBounds().width - invalid.x;
		}
		if(invalid.y + invalid.height > getBounds().height)
		{
			invalid.height = getBounds().height - invalid.y;
		}
		fenster::platformReleaseMutex(lock);
	}

	fenster::Rectangle Screen::grabInvalid()
	{
		fenster::platformAcquireMutex(lock);
		fenster::Rectangle ret = invalid;
		invalid = fenster::Rectangle();
		fenster::platformReleaseMutex(lock);
		return ret;
	}

	void Screen::showMenu(Menu* menu, fenster::Point position)
	{
		Component::addChild(menu);
		menu->setZIndex(10000);
		auto size = menu->getEffectivePreferredSize();
		menu->setBounds(fenster::Rectangle(position.x, position.y, size.width, size.height));
		menu->setFocused(true);
	}

	void Screen::hideMenus()
	{
		for(auto child: acquireChildren())
		{
			auto childAsMenu = dynamic_cast<Menu*>(child.component);
			if(childAsMenu)
				childAsMenu->close();
		}
		releaseChildren();
	}


}

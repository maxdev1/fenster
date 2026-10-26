// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/desktop/screen.hpp"
#include "components/window.hpp"
#include "components/menu.hpp"
#include "component_registry.hpp"
#include "components/cursor.hpp"
#include "server.hpp"

#include <cstring>

namespace fensterserver
{
	bool Screen::setDesktopPopup(Component* component, bool visible)
	{
		if(!visible)
		{
			if(component != desktopPopup) return false;
			dismissDesktopPopup(true);
			return true;
		}
		if(component != desktopPopup)
		{
			if(!taskbar || component->getParent() != taskbar || component->isWindow()) return false;
			dismissDesktopPopup(false);
			desktopPopup = component;
			component->setZIndex(10000);
			Component::addChild(component);
		}
		if(Cursor::focusedComponent != component->id)
			popupPreviousFocus = Cursor::focusedComponent;
		component->setVisible(true);
		Server::instance()->switchFocus(component);
		return true;
	}

	void Screen::dismissDesktopPopup(bool restoreFocus)
	{
		if(!getDesktopPopup()) return;
		auto source = ComponentRegistry::get(Cursor::focusedComponent);
		const bool ownedFocus = source && (source == desktopPopup || source->isChildOf(desktopPopup));
		const auto previous = popupPreviousFocus;
		popupPreviousFocus = -1;
		desktopPopup->setVisible(false);
		if(restoreFocus && ownedFocus)
			if(auto target = ComponentRegistry::get(previous))
				if(target->canHandleEvents()) Server::instance()->switchFocus(target);
	}

	Component* Screen::handleMouseEvent(MouseEvent& event)
	{
		if(event.type == fenster::MouseEventType::Press && getDesktopPopup() &&
		   !desktopPopup->getBounds().contains(event.position) &&
		   !(taskbar && taskbar->getBounds().contains(event.position)))
			dismissDesktopPopup(false);
		return Component::handleMouseEvent(event);
	}

	bool Screen::setTaskbar(Component* component, bool enabled)
	{
		if(enabled)
		{
			if(taskbar == component) return true;
			if(taskbar || !desktopCanvas || component->getParent() != desktopCanvas || component->isWindow()) return false;
			taskbar = component;
			component->setZIndex(9000);
			Component::addChild(component);
		}
		else
		{
			if(taskbar != component || !desktopCanvas) return false;
			taskbar = nullptr;
			component->setZIndex(1000);
			desktopCanvas->addChild(component);
		}
		markFor(COMPONENT_REQUIREMENT_LAYOUT);
		return true;
	}

	fenster::Rectangle Screen::getWindowArea()
	{
		auto size = getBounds();
		fenster::Rectangle area{0, 0, size.width, size.height};
		if(taskbar && taskbar->isVisible())
		{
			auto dock = taskbar->getBounds().clip(area);
			if(dock.width == area.width && dock.height > 0 && dock.y + dock.height == area.height)
				area.height = dock.y;
		}
		return area;
	}

	void Screen::layout()
	{
		Component::layout();
		for(auto child : acquireChildren())
			if(child.component->isWindow()) child.component->markFor(COMPONENT_REQUIREMENT_LAYOUT);
		releaseChildren();
	}

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
		if(comp == desktopPopup) { desktopPopup = nullptr; popupPreviousFocus = -1; }
		if(comp == taskbar) dismissDesktopPopup(false);
		if(comp == taskbar) taskbar = nullptr;
		if(comp == desktopCanvas) desktopCanvas = nullptr;
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
		ComponentRegistry::sendEvent(observerThread, &windowEvent, sizeof(fenster::WindowsEvent), SYS_TX_NONE);
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
		menu->setZIndex(10000);
		Component::addChild(menu);
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

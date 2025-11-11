// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/menu_item.hpp"

#include <server.hpp>
#include <libfenster/properties.hpp>
#include <util/cairo.hpp>

#include "menu.hpp"
#include "radio_button.hpp"

namespace fensterserver
{
	MenuItem::MenuItem():
		background(ARGB(50, 250, 250, 250)), insets(5, 10, 5, 10)
	{
		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
	}

	void MenuItem::paint()
	{
		if(!hasGraphics())
			return;

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();

		if(state.hovered)
		{
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(ARGB(255, 255, 255, 255)));
		}
		else
		{
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(background));
		}
		cairo_rectangle(cr, 0, 0, bounds.width, bounds.height);
		cairo_fill(cr);

		graphics.releaseContext();
	}

	void MenuItem::setBackground(fenster::ColorArgb color)
	{
		background = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		recheckGraphics();
	}

	fenster::ColorArgb MenuItem::getBackground()
	{
		return background;
	}

	void MenuItem::setTitleInternal(std::string title)
	{
		label.setTitle(title);
	}

	std::string MenuItem::getTitle()
	{
		return label.getTitle();
	}

	void MenuItem::layout()
	{
		auto bounds = getBounds();
		bounds.x = 0;
		bounds.y = 0;
		bounds -= insets;
		label.setBounds(bounds);
	}

	void MenuItem::update()
	{
		fenster::Dimension preferred = label.getPreferredSize();
		preferred.width += insets.left + insets.right;
		preferred.height += insets.top + insets.bottom;

		auto min = getMinimumSize();
		if(preferred.height < min.height)
			preferred.height = min.height;
		if(preferred.width < min.width)
			preferred.width = min.width;

		setPreferredSize(preferred);

		markParentFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	void MenuItem::addChild(Component* comp, ComponentChildReferenceType type)
	{
		if(!subMenu)
			subMenu = new Menu();
		subMenu->addChild(comp, type);
	}

	bool MenuItem::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Background)
		{
			*out = background;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	Component* MenuItem::handleMouseEvent(MouseEvent& event)
	{
		if(event.type == fenster::MouseEventType::Move)
			return this;

		if(event.type == fenster::MouseEventType::Enter)
		{
			state.hovered = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}

		if(event.type == fenster::MouseEventType::Leave)
		{
			state.hovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}

		if(event.type == fenster::MouseEventType::Press)
		{
			// In case this item is in a sub-menu, the parent is a Menu:
			auto parentMenu = dynamic_cast<Menu*>(this->getParent());
			if(parentMenu)
			{
				// If parent has different sub-menu open, close it
				auto openSubMenu = parentMenu->getOpenSubMenu();
				if(openSubMenu)
					openSubMenu->close();

				if(subMenu)
				{
					if(subMenu->getParent() != nullptr)
					{
						subMenu->close();
						return this;
					}

					auto nextToSelf = this->getLocationOnScreen();
					nextToSelf.x += this->getBounds().width;
					nextToSelf.y--;
					Server::instance()->screen->showMenu(subMenu, nextToSelf);
					parentMenu->setOpenSubMenu(subMenu);
					return subMenu;
				}

				this->fireAction();
				return this;
			}

			// Otherwise, parent is a MenuBar:
			if(subMenu)
			{
				if(subMenu->getParent() != nullptr)
				{
					// Catch press event, so the MenuBar item gets focused and the menu gets hidden
					return this;
				}

				auto belowSelf = this->getLocationOnScreen();
				belowSelf.y += this->getBounds().height;
				Server::instance()->screen->showMenu(subMenu, belowSelf);
				return subMenu;
			}

			this->fireAction();
			return this;
		}

		return nullptr;
	}

	void MenuItem::closeMenu()
	{
		if(subMenu)
			subMenu->close();
	}

	bool MenuItem::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Background)
		{
			setBackground(value);
			return true;
		}
		return Component::setNumericProperty(property, value);
	}
}

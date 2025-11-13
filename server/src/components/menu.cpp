// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/menu.hpp"

#include <layout/stack_layout.hpp>
#include <libfenster/properties.hpp>

#include "radio_button.hpp"

namespace fensterserver
{
	Menu::Menu():
		background(ARGB(255, 240, 240, 240))
	{
		auto layout = new StackLayout();
		layout->setPadding(fenster::Insets(1, 1, 1, 1));
		layout->setOrientation(fenster::Orientation::Vertical);
		Component::setLayout(layout);
	}

	void Menu::paint()
	{
		if(!hasGraphics())
			return;

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();

		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(background));
		cairo_rectangle(cr, 0, 0, getBounds().width, getBounds().height);
		cairo_fill(cr);

		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(_RGB(180, 180, 180)));
		cairo_rectangle(cr, 0, 0, getBounds().width, getBounds().height);
		cairo_set_line_width(cr, 1);
		cairo_stroke(cr);

		graphics.releaseContext();
	}

	void Menu::update()
	{
		fenster::Dimension totalPreferred;
		for(auto child: acquireChildren())
		{
			auto childSize = child.component->getEffectivePreferredSize();
			totalPreferred.width = std::max(totalPreferred.width, childSize.width);
			totalPreferred.height += childSize.height;
		}

		releaseChildren();

		totalPreferred.width = std::max(50, totalPreferred.width);
		totalPreferred.height = std::max(20, totalPreferred.height);
		setPreferredSize(totalPreferred);

		markFor(COMPONENT_REQUIREMENT_LAYOUT);
	}

	void Menu::layout()
	{
		auto size = getEffectivePreferredSize();
		auto pos = getBounds();
		this->setBounds(fenster::Rectangle(pos.x, pos.y, size.width, size.height));

		Component::layout();
	}


	void Menu::setBackground(fenster::ColorArgb color)
	{
		background = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		recheckGraphics();
	}

	fenster::ColorArgb Menu::getBackground()
	{
		return background;
	}

	void Menu::setFocusedInternal(bool focused)
	{
		FocusableComponent::setFocusedInternal(focused);
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	Component* Menu::handleMouseEvent(MouseEvent& event)
	{
		Component* handledByChild = Component::handleMouseEvent(event);
		if(handledByChild)
			return handledByChild;

		return this;
	}

	bool Menu::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Background)
		{
			*out = background;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool Menu::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Background)
		{
			setBackground(value);
			return true;
		}
		return Component::setNumericProperty(property, value);
	}

	void Menu::close()
	{
		for(auto child: acquireChildren())
		{
			auto childMenuItem = dynamic_cast<MenuItem*>(child.component);
			if(childMenuItem)
				childMenuItem->closeMenu();
		}
		releaseChildren();

		auto parent = this->getParent();
		if(parent)
			parent->removeChild(this);
	}

}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/panel.hpp"
#include <libfenster/properties.hpp>
#include "util/cairo.hpp"

namespace fensterserver
{
	void Panel::paint()
	{
		if(!hasGraphics())
			return;

		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();

		auto color = selected && selectedBackground ? selectedBackground : hovered && hoverBackground ? hoverBackground : background;
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(color));
		auto box = getBounds();
		cairo_rounded_rectangle(cr, .5, .5, box.width - 1, box.height - 1, cornerRadius);
		cairo_fill_preserve(cr);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(border)); cairo_set_line_width(cr, 1); cairo_stroke(cr);

		graphics.releaseContext();
	}

	Component* Panel::handleMouseEvent(MouseEvent& event)
	{
		auto handled = Component::handleMouseEvent(event);
		if(handled && handled != this) return handled;
		if(event.type == fenster::MouseEventType::Leave) { hovered = false; markFor(COMPONENT_REQUIREMENT_PAINT); }
		else if(event.type == fenster::MouseEventType::Enter) { hovered = true; markFor(COMPONENT_REQUIREMENT_PAINT); }
		if(hoverBackground && (event.type == fenster::MouseEventType::Move || event.type == fenster::MouseEventType::Enter || event.type == fenster::MouseEventType::Leave)) return this;
		return handled;
	}

	void Panel::setBackground(   fenster::ColorArgb color)
	{
		if(background == color) return;
		background = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		recheckGraphics();
	}

	   fenster::ColorArgb Panel::getBackground()
	{
		return background;
	}

	bool Panel::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		switch(property) {
			case fenster::ComponentProperty::CornerRadius: *out = cornerRadius; return true;
			case fenster::ComponentProperty::BorderColor: *out = border; return true;
			case fenster::ComponentProperty::HoverBackground: *out = hoverBackground; return true;
			case fenster::ComponentProperty::SelectedBackground: *out = selectedBackground; return true;
			case fenster::ComponentProperty::Selected: *out = selected; return true;
			default: break;
		}
		if(property == fenster::ComponentProperty::Background)
		{
			*out = background;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool Panel::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Focusable) keyboardFocusable = value != 0;
		uint32_t old;
		if(getNumericProperty(property, &old) && old == value) return true;
		switch(property)
		{
			case fenster::ComponentProperty::CornerRadius: cornerRadius = std::min<uint32_t>(value, 64); break;
			case fenster::ComponentProperty::BorderColor: border = value; break;
			case fenster::ComponentProperty::HoverBackground: hoverBackground = value; break;
			case fenster::ComponentProperty::SelectedBackground: selectedBackground = value; break;
			case fenster::ComponentProperty::Selected: selected = value; break;
			default: return property == fenster::ComponentProperty::Background ? (setBackground(value), true) : Component::setNumericProperty(property, value);
		}
		markFor(COMPONENT_REQUIREMENT_PAINT); recheckGraphics(); return true;
	}
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/button.hpp"
#include "components/window.hpp"
#include "events/mouse_event.hpp"
#include "util/cairo.hpp"

#include <libproperties/properties.hpp>
#include <libfenster/properties.hpp>
#include <libfenster/font/text_alignment.hpp>
#include <math.h>


namespace fensterserver
{
	Button::Button() :
		insets(fenster::Insets(5, 8, 5, 8))
	{
		enabled = true;
		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		label.setAlignment(fenster::TextAlignment::CENTER);
		label.setColor(_RGB(10, 10, 15));
	}

	/**
	 * Layouts the button.
	 */
	void Button::update()
	{
		fenster::Dimension preferred = label.getPreferredSize();
		preferred.width += insets.left + insets.right + (icon != fenster::Icon::None ? 26 : 0);
		preferred.height += insets.top + insets.bottom;

		auto min = getMinimumSize();
		if(preferred.height < min.height)
			preferred.height = min.height;
		if(preferred.width < min.width)
			preferred.width = min.width;

		setPreferredSize(preferred);

		markParentFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	void Button::layout()
	{
		auto box = getBounds(); box.x = 0; box.y = 0; box -= insets;
		if(icon != fenster::Icon::None && !getTitle().empty()) { box.x += 26; box.width -= 26; }
		label.setBounds(box);
		label.setAlignment(alignment);
		label.setColor(!enabled ? _RGB(170, 178, 190) : selected ? _RGB(62, 81, 196) : _RGB(48, 57, 74));
	}

	void Button::paint()
	{
		auto cr = graphics.acquireContext();
		if(!cr) return;
		clearSurface();
		auto box = getBounds();
		const bool ghost = variant == fenster::ButtonVariant::Ghost;
		auto fill = selected ? _RGB(233, 237, 255) : ghost ? ARGB(0, 0, 0, 0) : _RGB(255, 255, 255);
		auto border = ghost ? ARGB(0, 0, 0, 0) : _RGB(218, 224, 234);
		if(enabled && state.hovered) fill = selected ? _RGB(223, 229, 253) : _RGB(238, 242, 248);
		if(enabled && state.pressed && state.hovered) fill = _RGB(220, 227, 240);
		if(!enabled) { fill = ghost ? ARGB(0, 0, 0, 0) : _RGB(248, 250, 252); border = ghost ? border : _RGB(233, 237, 243); }
		cairo_rounded_rectangle(cr, .5, .5, box.width - 1, box.height - 1, 7);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(fill)); cairo_fill_preserve(cr);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(border)); cairo_set_line_width(cr, 1); cairo_stroke(cr);
		// Subtle focus inset keeps selection distinct from the transient pointer state.
		if(focused && enabled && !ghost) {
			cairo_rounded_rectangle(cr, 2.5, 2.5, box.width - 5, box.height - 5, 5);
			cairo_set_source_rgba(cr, .36, .43, .85, .45); cairo_stroke(cr);
		}
		if(icon != fenster::Icon::None) {
			double x = getTitle().empty() ? (box.width - 18) / 2. : 12;
			fenster::paintIcon(cr, icon, x, (box.height - 18) / 2., 18,
				!enabled ? _RGB(179, 187, 200) : selected ? _RGB(76, 91, 205) : _RGB(91, 105, 125));
		}
		graphics.releaseContext();
	}

	Component* Button::handleMouseEvent(MouseEvent& event)
	{
		if(event.type == fenster::MouseEventType::Move) return this;
		// Cleanup is delivered even if a button was disabled during a press.
		if(event.type == fenster::MouseEventType::Leave) {
			state.hovered = false; markFor(COMPONENT_REQUIREMENT_PAINT);
			Component::handleMouseEvent(event); return this;
		}
		if(event.type == fenster::MouseEventType::DragRelease) {
			state.pressed = false; armed = false; markFor(COMPONENT_REQUIREMENT_PAINT); return this;
		}
		if(!enabled) return event.type == fenster::MouseEventType::Move ? this : nullptr;
		if(event.type == fenster::MouseEventType::Enter) { state.hovered = true; }
		else if(event.type == fenster::MouseEventType::Press) {
			if(!(event.buttons & fenster::MouseButton::Button1)) return this;
			armed = true; state.pressed = true; state.hovered = true;
		}
		else if(event.type == fenster::MouseEventType::Drag) {
			auto box = getBounds(); box.x = box.y = 0;
			state.hovered = box.contains(event.position);
		}
		else if(event.type == fenster::MouseEventType::Release) {
			auto box = getBounds(); box.x = box.y = 0;
			state.hovered = box.contains(event.position);
			// Only a completed primary press is an action; ending capture alone never clicks.
			if(armed && state.hovered) fireAction();
			armed = false;
			state.pressed = false;
		}
		else if(event.type != fenster::MouseEventType::Move) return Component::handleMouseEvent(event);
		markFor(COMPONENT_REQUIREMENT_PAINT);
		Component::handleMouseEvent(event);
		return this;
	}

	Component* Button::handleKeyEvent(KeyEvent& event)
	{
		if(enabled && !event.info.ctrl && !event.info.alt && (event.info.key == "KEY_ENTER" || event.info.key == "KEY_SPACE")) {
			if(event.info.pressed) { armed = true; state.pressed = true; }
			else { if(armed) fireAction(); armed = false; state.pressed = false; }
			markFor(COMPONENT_REQUIREMENT_PAINT); return this;
		}
		return Component::handleKeyEvent(event);
	}

	void Button::setFocusedInternal(bool focused)
	{
		FocusableComponent::setFocusedInternal(focused);
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Button::isFocusableDefault() const
	{
		return enabled;
	}

	void Button::setTitleInternal(std::string title)
	{
		this->label.setTitle(title);
	}

	std::string Button::getTitle()
	{
		return this->label.getTitle();
	}

	void Button::setEnabled(bool enabled)
	{
		this->enabled = enabled;
		if(!enabled) { state = {}; armed = false; }
		layout();
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Button::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Icon) { *out = static_cast<uint32_t>(icon); return true; }
		if(property == fenster::ComponentProperty::Selected) { *out = selected; return true; }
		if(property == fenster::ComponentProperty::Enabled)
		{
			*out = enabled;
			return true;
		}
		else if(property == fenster::ComponentProperty::Variant)
		{
			*out = (uint32_t) variant;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool Button::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::FontSize) {
			label.setNumericProperty(property, value); markFor(COMPONENT_REQUIREMENT_ALL); return true;
		}
		if(property == fenster::ComponentProperty::Icon) { icon = static_cast<fenster::Icon>(value); markFor(COMPONENT_REQUIREMENT_ALL); return true; }
		if(property == fenster::ComponentProperty::Selected) { selected = value; layout(); markFor(COMPONENT_REQUIREMENT_PAINT); return true; }
		if(property == fenster::ComponentProperty::Alignment) { alignment = static_cast<fenster::TextAlignment>(value); markFor(COMPONENT_REQUIREMENT_LAYOUT); return true; }
		if(property == fenster::ComponentProperty::Enabled)
		{
			setEnabled(value);
			markFor(COMPONENT_REQUIREMENT_ALL);
			return true;
		}
		else if(property == fenster::ComponentProperty::Variant)
		{
			variant = (fenster::ButtonVariant) value;
			markFor(COMPONENT_REQUIREMENT_ALL);
			return true;
		}

		return Component::setNumericProperty(property, value);
	}
}

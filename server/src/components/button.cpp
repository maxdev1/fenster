// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/button.hpp"
#include "components/window.hpp"
#include "events/mouse_event.hpp"

#include <libproperties/properties.hpp>
#include <libfenster/properties.hpp>
#include <libfenster/font/text_alignment.hpp>
#include <math.h>


namespace fensterserver
{
	enum class ButtonRenderState: int
	{
		Default = 0,
		Hover = 1,
		Pressed = 2,
		Disabled = 3
	};

	enum class ButtonStateColorType: int
	{
		BACKGROUND = 0,
		BORDER = 1,
		BORDER_FOCUSED = 2
	};

	std::map<fenster::ButtonVariant,
	         std::map<ButtonRenderState,
	                  std::map<ButtonStateColorType, fenster::ColorArgb>
	         >
	> BUTTON_STYLES = {
			{fenster::ButtonVariant::Default, {
					 {
							 ButtonRenderState::Default, {
									 {ButtonStateColorType::BACKGROUND, _RGB(248, 248, 248)},
									 {ButtonStateColorType::BORDER, _RGB(200, 200, 200)},
									 {ButtonStateColorType::BORDER_FOCUSED, _RGB(84, 149, 255)}
							 }
					 },
					 {
							 ButtonRenderState::Hover, {
									 {ButtonStateColorType::BACKGROUND, _RGB(255, 255, 255)},
									 {ButtonStateColorType::BORDER, _RGB(193, 193, 193)},
									 {ButtonStateColorType::BORDER_FOCUSED, _RGB(84, 149, 255)}
							 }
					 },
					 {
							 ButtonRenderState::Pressed, {
									 {ButtonStateColorType::BACKGROUND, _RGB(230, 230, 230)},
									 {ButtonStateColorType::BORDER, _RGB(193, 193, 193)},
									 {ButtonStateColorType::BORDER_FOCUSED, _RGB(84, 149, 255)}
							 }
					 },
					 {
							 ButtonRenderState::Disabled, {
									 {ButtonStateColorType::BACKGROUND, _RGB(249, 249, 249)},
									 {ButtonStateColorType::BORDER, _RGB(234, 234, 234)},
									 {ButtonStateColorType::BORDER_FOCUSED, _RGB(234, 234, 234)}
							 }
					 }
			 }},
			{fenster::ButtonVariant::Ghost, {
					 {
							 ButtonRenderState::Default, {
									 {ButtonStateColorType::BACKGROUND, ARGB(0, 0, 0, 0)},
									 {ButtonStateColorType::BORDER, ARGB(0, 0, 0, 0)},
									 {ButtonStateColorType::BORDER_FOCUSED, ARGB(0, 0, 0, 0)}
							 }
					 },
					 {
							 ButtonRenderState::Hover, {
									 {ButtonStateColorType::BACKGROUND, _RGB(255, 255, 255)},
									 {ButtonStateColorType::BORDER, _RGB(193, 193, 193)},
									 {ButtonStateColorType::BORDER_FOCUSED, _RGB(84, 149, 255)}
							 }
					 },
					 {
							 ButtonRenderState::Pressed, {
									 {ButtonStateColorType::BACKGROUND, _RGB(230, 230, 230)},
									 {ButtonStateColorType::BORDER, _RGB(193, 193, 193)},
									 {ButtonStateColorType::BORDER_FOCUSED, _RGB(84, 149, 255)}
							 }
					 },
					 {
							 ButtonRenderState::Disabled, {
									 {ButtonStateColorType::BACKGROUND, ARGB(0, 0, 0, 0)},
									 {ButtonStateColorType::BORDER, ARGB(0, 0, 0, 0)},
									 {ButtonStateColorType::BORDER_FOCUSED, ARGB(0, 0, 0, 0)}
							 }
					 }
			 }}
	};


	Button::Button() :
		insets(fenster::Insets(5, 10, 5, 10))
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

	void Button::layout()
	{
		auto bounds = getBounds();
		bounds.x = 0;
		bounds.y = 0;
		bounds -= insets;
		label.setBounds(bounds);
	}

	void Button::paint()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();

		auto styleData = BUTTON_STYLES[variant];
		auto stateData = enabled
			                 ? state.pressed
				                   ? styleData[ButtonRenderState::Pressed]
				                   : (state.hovered
					                      ? styleData[ButtonRenderState::Hover]
					                      : styleData[ButtonRenderState::Default])
			                 : styleData[ButtonRenderState::Disabled];
		fenster::ColorArgb background = stateData[ButtonStateColorType::BACKGROUND];
		fenster::ColorArgb border = stateData[focused
			                                      ? ButtonStateColorType::BORDER_FOCUSED
			                                      : ButtonStateColorType::BORDER];

		double offsetX = 0.5;
		double offsetY = 0.5;
		double borderRadius = 5;

		auto bounds = getBounds();
		double width = bounds.width - 1;
		double height = bounds.height - 1;
		double degrees = M_PI / 180.0;

		cairo_new_sub_path(cr);
		cairo_arc(cr, offsetX + width - borderRadius, offsetY + borderRadius, borderRadius, -90 * degrees, 0 * degrees);
		cairo_arc(cr, offsetX + width - borderRadius, offsetY + height - borderRadius, borderRadius, 0 * degrees,
		          90 * degrees);
		cairo_arc(cr, offsetX + borderRadius, offsetY + height - borderRadius, borderRadius, 90 * degrees,
		          180 * degrees);
		cairo_arc(cr, offsetX + borderRadius, offsetY + borderRadius, borderRadius, 180 * degrees, 270 * degrees);
		cairo_close_path(cr);

		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(background));
		cairo_fill_preserve(cr);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(border));
		cairo_set_line_width(cr, 1);
		cairo_stroke(cr);

		graphics.releaseContext();
	}

	Component* Button::handleMouseEvent(MouseEvent& me)
	{
		if(!enabled)
			return nullptr;

		if(me.type == fenster::MouseEventType::Enter)
		{
			state.hovered = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Leave)
		{
			state.hovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Press)
		{
			state.pressed = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			return this;
		}
		else if(me.type == fenster::MouseEventType::Release || me.type == fenster::MouseEventType::DragRelease)
		{
			state.pressed = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);

			if(me.type == fenster::MouseEventType::Release)
			{
				if(me.position.x >= 0 && me.position.y >= 0 && me.position.x < getBounds().width && me.position.y <
				   getBounds().height)
				{
					fireAction();
				}
			}
			return this;
		}

		return nullptr;
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
		label.setColor(enabled ? _RGB(10, 10, 15) : _RGB(80, 80, 80));
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Button::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
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

		return false;
	}

	bool Button::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
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

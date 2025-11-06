// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/window.hpp"
#include "components/cursor.hpp"
#include "events/mouse_event.hpp"
#include "server.hpp"
#include "component_registry.hpp"

#include <cairo/cairo.h>
#include <libfenster/font/font_manager.hpp>
#include <libfenster/properties.hpp>
#include <cmath>

namespace fensterserver
{
	Window::Window() :
		backgroundColor(_RGB(244, 244, 248)), borderWidth(DEFAULT_BORDER_WIDTH), cornerSize(DEFAULT_CORNER_SIZE),
		resizeMode(RESIZE_MODE_NONE)
	{
		focused = false;
		visible = false;
		resizable = true;
		crossHovered = false;
		crossPressed = false;

		shadowSize = 10;
		titleHeight = 40;

		graphics.setAverageFactor(250);

		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		Component::addChild(&panel, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		setMinimumSize(fenster::Dimension(200, 80));

		panel.setBackground(ARGB(0, 0, 0, 0));
	}

	void Window::layout()
	{
		fenster::Rectangle bounds = getBounds();
		label.setBounds(fenster::Rectangle(shadowSize + 12, shadowSize, bounds.width - shadowSize - 20, titleHeight));
		panel.setBounds(fenster::Rectangle(shadowSize, shadowSize + titleHeight, bounds.width - shadowSize * 2,
		                                   bounds.height - titleHeight - shadowSize * 2));
		crossBounds = fenster::Rectangle(bounds.width - 47, 15, 30, 30);
	}

	/**
	 * TODO: Remove subpanel method and let the client do the work here
	 */
	void Window::addChild(Component* component, ComponentChildReferenceType type)
	{
		panel.addChild(component);
	}

	void Window::setLayout(Layout* layout)
	{
		panel.setLayout(layout);
	}

	Layout* Window::getLayout() const
	{
		return panel.getLayout();
	}

	void roundedRectangle(cairo_t* cr, double x, double y, double width, double height, double radius)
	{
		double degrees = M_PI / 180.0;
		cairo_new_sub_path(cr);
		cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
		cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
		cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
		cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
		cairo_close_path(cr);
	}

	void Window::paintBackground(cairo_t* cr) const
	{
		auto bounds = getBounds();

		double shadowAlpha = 0.003;
		double shadowAlphaAdd = 0.003;
		double addIncr;
		if(isFocused())
		{
			addIncr = 0.003;
		}
		else
		{
			addIncr = 0.001;
		}
		for(int i = 0; i < shadowSize + 4; i++)
		{
			roundedRectangle(cr, i, i + 3, bounds.width - 2 * i, bounds.height - 2 * i, shadowSize);
			cairo_set_source_rgba(cr, 0, 0, 0, shadowAlpha);
			cairo_stroke(cr);
			shadowAlpha += shadowAlphaAdd;
			shadowAlphaAdd += addIncr;
		}

		int borderRadius = 5;
		roundedRectangle(cr, shadowSize, shadowSize, bounds.width - 2 * shadowSize, bounds.height - 2 * shadowSize,
		                 borderRadius);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(backgroundColor));
		cairo_fill(cr);
	}

	void Window::paintControls(cairo_t* cr) const
	{
		if(crossHovered)
		{
			cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(ARGB(20, 50, 50, 50)));
			cairo_rectangle(cr, crossBounds.x, crossBounds.y, crossBounds.width, crossBounds.height);
			cairo_fill(cr);
		}

		if(crossPressed && crossHovered)
			cairo_set_source_rgba(cr, 0.0, 0.4, 0.8, 1);
		else if(crossHovered)
			cairo_set_source_rgba(cr, 0.1, 0.5, 0.9, 1);
		else
			cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 1);

		int crossPadding = 11;
		cairo_set_line_width(cr, 1.5);
		cairo_move_to(cr, crossBounds.x + crossPadding, crossBounds.y + crossPadding);
		cairo_line_to(cr, crossBounds.x + crossBounds.width - crossPadding,
		              crossBounds.y + crossBounds.height - crossPadding);
		cairo_stroke(cr);
		cairo_move_to(cr, crossBounds.x + crossBounds.width - crossPadding, crossBounds.y + crossPadding);
		cairo_line_to(cr, crossBounds.x + crossPadding, crossBounds.y + crossBounds.height - crossPadding);
		cairo_stroke(cr);
	}

	void Window::paint()
	{
		cairo_t* cr = graphics.acquireContext();
		if(!cr)
			return;

		clearSurface();
		paintBackground(cr);
		paintControls(cr);
		graphics.releaseContext();
	}

	void Window::setFocusedInternal(bool focused)
	{
		FocusableComponent::setFocusedInternal(focused);
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void Window::dragOrResize(MouseEvent& me)
	{
		if(resizeMode == RESIZE_MODE_MOVE)
		{
			fenster::Point newLocation = me.screenPosition - pressPoint;
			setBounds({newLocation.x, newLocation.y, pressBounds.width, pressBounds.height});
			Cursor::set("drag");
			return;
		}

		fenster::Point newLocation = me.screenPosition - pressPoint;

		int left = pressBounds.x;
		int top = pressBounds.y;
		int right = pressBounds.x + pressBounds.width;
		int bottom = pressBounds.y + pressBounds.height;

		if(resizeMode & RESIZE_MODE_LEFT)
			left = newLocation.x;
		if(resizeMode & RESIZE_MODE_RIGHT)
			right = newLocation.x + pressBounds.width;
		if(resizeMode & RESIZE_MODE_TOP)
			top = newLocation.y;
		if(resizeMode & RESIZE_MODE_BOTTOM)
			bottom = newLocation.y + pressBounds.height;

		auto minimumSize = getMinimumSize();
		if(right - left < minimumSize.width)
		{
			if(resizeMode & RESIZE_MODE_LEFT)
				left = right - minimumSize.width;
			else
				right = left + minimumSize.width;
		}
		if(bottom - top < minimumSize.height)
		{
			if(resizeMode & RESIZE_MODE_TOP)
				top = bottom - minimumSize.height;
			else
				bottom = top + minimumSize.height;
		}

		fenster::Rectangle r;
		r.x = left;
		r.y = top;
		r.width = right - left;
		r.height = bottom - top;

		setBounds(r);
	}

	void Window::startDragOrResize(MouseEvent& me)
	{
		auto currentBounds = getBounds();
		pressPoint = me.position;
		pressBounds = currentBounds;

		resizeMode = RESIZE_MODE_NONE;

		if(resizable)
		{
			if((pressPoint.x < shadowSize + cornerSize / 2) && (pressPoint.x > shadowSize - cornerSize / 2) && (
				   pressPoint.y < cornerSize) && (pressPoint.y > shadowSize - cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_TOP | RESIZE_MODE_LEFT;
			}
			else if((pressPoint.x > currentBounds.width - shadowSize - cornerSize / 2) && (
				        pressPoint.x < currentBounds.width - shadowSize + cornerSize / 2) && (
				        pressPoint.y < cornerSize) && (pressPoint.y > shadowSize - cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_TOP | RESIZE_MODE_RIGHT;
			}
			else if((pressPoint.x < shadowSize + cornerSize / 2) && (pressPoint.x > shadowSize - cornerSize / 2)
			        &&
			        (pressPoint.y > currentBounds.height - shadowSize - cornerSize / 2) && (
				        pressPoint.y < currentBounds.height - shadowSize + cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_BOTTOM | RESIZE_MODE_LEFT;
			}
			else if((pressPoint.x > currentBounds.width - shadowSize - cornerSize / 2) && (
				        pressPoint.x < currentBounds.width - shadowSize + cornerSize / 2) && (
				        pressPoint.y > currentBounds.height - shadowSize - cornerSize / 2) && (
				        pressPoint.y < currentBounds.height - shadowSize + cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_BOTTOM | RESIZE_MODE_RIGHT;
			}
			else if(pressPoint.y < shadowSize + borderWidth / 2 && pressPoint.y > shadowSize - borderWidth / 2)
			{
				resizeMode = RESIZE_MODE_TOP;
			}
			else if(pressPoint.x < shadowSize + borderWidth / 2 && pressPoint.x > shadowSize - borderWidth / 2)
			{
				resizeMode = RESIZE_MODE_LEFT;
			}
			else if((pressPoint.y > currentBounds.height - shadowSize - borderWidth / 2) && (
				        pressPoint.y < currentBounds.height - shadowSize + borderWidth / 2))
			{
				resizeMode = RESIZE_MODE_BOTTOM;
			}
			else if((pressPoint.x > currentBounds.width - shadowSize - borderWidth / 2) && (
				        pressPoint.x < currentBounds.width - shadowSize + borderWidth / 2))
			{
				resizeMode = RESIZE_MODE_RIGHT;
			}
		}

		if(resizeMode == RESIZE_MODE_NONE)
		{
			if(pressPoint.y < titleHeight)
			{
				resizeMode = RESIZE_MODE_MOVE;
				Cursor::set("drag");
			}
		}
	}

	Component* Window::handleMouseEvent(MouseEvent& me)
	{
		if(me.type == fenster::MouseEventType::Drag)
		{
			if(crossPressed)
			{
				crossHovered = crossBounds.contains(me.position);
				markFor(COMPONENT_REQUIREMENT_PAINT);
			}
			else if(resizeMode != RESIZE_MODE_NONE)
			{
				dragOrResize(me);
			}
			return this;
		}

		// Let child components handle other input first
		Component* handledByChild = Component::handleMouseEvent(me);
		if(handledByChild)
			return handledByChild;

		// Handle mouse events
		if(me.type == fenster::MouseEventType::Move)
		{
			if(resizable)
			{
				fenster::Rectangle currentBounds = getBounds();

				fenster::Point pos = me.position;
				if((pos.x < shadowSize + cornerSize / 2) && (pos.x > shadowSize - cornerSize / 2) && (
					   pos.y < cornerSize) &&
				   (pos.y > shadowSize - cornerSize / 2))
				{
					// Top left corner
					Cursor::set("resize-nwes");
				}
				else if((pos.x > currentBounds.width - shadowSize - cornerSize / 2) && (
					        pos.x < currentBounds.width - shadowSize + cornerSize / 2) && (pos.y < cornerSize) && (
					        pos.y > shadowSize - cornerSize / 2))
				{
					// Top right corner
					Cursor::set("resize-nesw");
				}
				else if((pos.x < shadowSize + cornerSize / 2) && (pos.x > shadowSize - cornerSize / 2) && (
					        pos.y > currentBounds.height - shadowSize - cornerSize / 2) && (
					        pos.y < currentBounds.height - shadowSize + cornerSize / 2))
				{
					// Bottom left corner
					Cursor::set("resize-nesw");
				}
				else if((pos.x > currentBounds.width - shadowSize - cornerSize / 2) && (
					        pos.x < currentBounds.width - shadowSize + cornerSize / 2) && (
					        pos.y > currentBounds.height - shadowSize - cornerSize / 2) && (
					        pos.y < currentBounds.height - shadowSize + cornerSize / 2))
				{
					// Bottom right corner
					Cursor::set("resize-nwes");
				}
				else if(pos.y < shadowSize + borderWidth / 2 && pos.y > shadowSize - borderWidth / 2)
				{
					// Top edge
					Cursor::set("resize-ns");
				}
				else if(pos.x < shadowSize + borderWidth / 2 && pos.x > shadowSize - borderWidth / 2)
				{
					// Left edge
					Cursor::set("resize-ew");
				}
				else if((pos.y > currentBounds.height - shadowSize - borderWidth / 2) && (
					        pos.y < currentBounds.height - shadowSize + borderWidth / 2))
				{
					// Bottom edge
					Cursor::set("resize-ns");
				}
				else if((pos.x > currentBounds.width - shadowSize - borderWidth / 2) && (
					        pos.x < currentBounds.width - shadowSize + borderWidth / 2))
				{
					// Right edge
					Cursor::set("resize-ew");
				}
				else
				{
					Cursor::set("default");
				}
			}

			if(crossBounds.contains(me.position))
			{
				crossHovered = true;
				markFor(COMPONENT_REQUIREMENT_PAINT);
			}
			else
			{
				if(crossHovered)
				{
					markFor(COMPONENT_REQUIREMENT_PAINT);
				}
				crossHovered = false;
			}
		}
		else if(me.type == fenster::MouseEventType::Press)
		{
			// Press on the cross
			if(crossBounds.contains(me.position))
			{
				crossPressed = true;
				markFor(COMPONENT_REQUIREMENT_PAINT);
				return this;
			}
			else
			{
				startDragOrResize(me);
			}
		}
		else if(me.type == fenster::MouseEventType::DragRelease)
		{
			crossPressed = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			Cursor::set("default");

			resizeMode = RESIZE_MODE_NONE;
		}
		else if(me.type == fenster::MouseEventType::Release)
		{
			if(crossBounds.contains(me.position))
			{
				this->close();
				return this;
			}

			resizeMode = RESIZE_MODE_NONE;
		}
		else if(me.type == fenster::MouseEventType::Leave)
		{
			Cursor::set("default");
			fenster::platformLog("Window sets default cursor");
		}

		return this;
	}

	void Window::close()
	{
		this->callForListeners(fenster::ComponentEventType::Close, [](EventListenerInfo& info)
		{
			fenster::ComponentCloseEvent posted_event;
			posted_event.header.type = fenster::ComponentEventType::Close;
			posted_event.header.component_id = info.component_id;
			platformSendMessage(info.target_thread, &posted_event, sizeof(fenster::ComponentCloseEvent), SYS_TX_NONE);
		});

		setVisible(false);
	}

	bool Window::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Resizable)
		{
			*out = resizable;
			return true;
		}

		if(property == fenster::ComponentProperty::Focused)
		{
			*out = focused ? 1 : 0;
			return true;
		}

		return Component::getNumericProperty(property, out);
	}

	bool Window::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Resizable)
		{
			resizable = value;
			return true;
		}

		if(property == fenster::ComponentProperty::Background)
		{
			backgroundColor = value;

			uint32_t avg = (ARGB_A_FROM(value) + ARGB_G_FROM(value) + ARGB_B_FROM(value)) / 3;
			label.setColor(avg > 128 ? ARGB(255, 0, 0, 0) : ARGB(255, 255, 255, 255));
			return true;
		}

		return Component::setNumericProperty(property, value);
	}

	void Window::setTitleInternal(std::string title)
	{
		label.setTitle(title);
	}

	std::string Window::getTitle()
	{
		return label.getTitle();
	}
}

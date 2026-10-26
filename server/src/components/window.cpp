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

#include "layout/fill_layout.hpp"

namespace fensterserver
{
	Window::Window() :
		backgroundColor(_RGB(250, 251, 253)), borderWidth(DEFAULT_BORDER_WIDTH), cornerSize(DEFAULT_CORNER_SIZE),
		resizeMode(RESIZE_MODE_NONE)
	{
		focused = false;
		visible = false;
		resizable = true;

		shadowSize = 10;
		titleHeight = 40;

		graphics.setAverageFactor(250);

		Component::addChild(&label, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		Component::addChild(&panel, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		setMinimumSize(fenster::Dimension(200, 80));

		panel.setBackground(ARGB(0, 0, 0, 0));
		panel.layoutPolicyHorizontal = LayoutingPolicy::Stretch;
		panel.layoutPolicyVertical = LayoutingPolicy::Stretch;
	}

	void Window::layout()
	{
		if(fullscreen) setBounds(getFullscreenBounds());
		fenster::Rectangle bounds = getBounds();
		crossBounds = fenster::Rectangle(bounds.width - 47, 15, 30, 30);
		int controlsLeft = crossBounds.x;
		fullscreenBounds = {};
		minimizeBounds = {};
		if(maximizeButtonVisible)
			fullscreenBounds = fenster::Rectangle(controlsLeft -= 34, 15, 30, 30);
		if(minimizeButtonVisible)
			minimizeBounds = fenster::Rectangle(controlsLeft -= 34, 15, 30, 30);
		label.setBounds(fenster::Rectangle(shadowSize + 12, shadowSize,
		                                 std::max(0, controlsLeft - shadowSize - 20), titleHeight));

		int menuBarHeight = 30;
		if(menuBar)
			menuBar->setBounds(fenster::Rectangle(shadowSize,
			                                      shadowSize + titleHeight,
			                                      bounds.width - shadowSize * 2,
			                                      menuBarHeight));

		panel.setBounds(fenster::Rectangle(shadowSize, shadowSize + titleHeight + (menuBar ? menuBarHeight : 0),
		                                   bounds.width - shadowSize * 2,
		                                   bounds.height - titleHeight - shadowSize * 2 - (menuBar ? menuBarHeight : 0)));
	}

	void Window::setMenuBar(MenuBar* menuBar)
	{
		this->menuBar = menuBar;
		Component::addChild(menuBar);
	}

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

		int borderRadius = 9;
		roundedRectangle(cr, shadowSize, shadowSize, bounds.width - 2 * shadowSize, bounds.height - 2 * shadowSize,
		                 borderRadius);
		cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(backgroundColor));
		cairo_fill_preserve(cr);
		cairo_set_source_rgba(cr, .56, .61, .70, isFocused() ? .28 : .16); cairo_set_line_width(cr, 1); cairo_stroke(cr);
		cairo_set_source_rgba(cr, .85, .88, .93, .65);
		cairo_move_to(cr, shadowSize, shadowSize + titleHeight - .5);
		cairo_line_to(cr, bounds.width - shadowSize, shadowSize + titleHeight - .5); cairo_stroke(cr);
	}

	void Window::paintControls(cairo_t* cr) const
	{
		for(auto control : {Control::Minimize, Control::Fullscreen, Control::Close})
		{
			if((control == Control::Minimize && !minimizeButtonVisible) ||
			   (control == Control::Fullscreen && !maximizeButtonVisible)) continue;
			const auto& box = control == Control::Close ? crossBounds :
			                  control == Control::Fullscreen ? fullscreenBounds : minimizeBounds;
			const bool enabled = control == Control::Close ||
			                     (control == Control::Fullscreen ? resizable : showInTaskbar);
			const bool hovered = enabled && hoveredControl == control;
			const bool pressed = hovered && pressedControl == control;
			if(hovered)
			{
				if(control == Control::Close) cairo_set_source_rgba(cr, .96, .88, .89, 1);
				else cairo_set_source_rgba(cr, .88, .91, .96, 1);
				roundedRectangle(cr, box.x, box.y, box.width, box.height, 7);
				cairo_fill(cr);
			}
			if(control == Control::Close && hovered)
				cairo_set_source_rgba(cr, pressed ? .65 : .78, .20, .24, 1);
			else
				cairo_set_source_rgba(cr, pressed ? .20 : .40, pressed ? .25 : .40, pressed ? .35 : .40,
				                      enabled ? 1 : .3);
			cairo_set_line_width(cr, 1.5);
			if(control == Control::Close)
			{
				cairo_move_to(cr, box.x + 11, box.y + 11);
				cairo_line_to(cr, box.x + 19, box.y + 19);
				cairo_move_to(cr, box.x + 19, box.y + 11);
				cairo_line_to(cr, box.x + 11, box.y + 19);
			}
			else if(control == Control::Minimize)
			{
				cairo_move_to(cr, box.x + 10, box.y + 19);
				cairo_line_to(cr, box.x + 20, box.y + 19);
			}
			else if(fullscreen)
			{
				cairo_move_to(cr, box.x + 13, box.y + 11);
				cairo_line_to(cr, box.x + 13, box.y + 9);
				cairo_line_to(cr, box.x + 21, box.y + 9);
				cairo_line_to(cr, box.x + 21, box.y + 17);
				cairo_line_to(cr, box.x + 19, box.y + 17);
				cairo_rectangle(cr, box.x + 10, box.y + 12, 8, 8);
			}
			else cairo_rectangle(cr, box.x + 10, box.y + 10, 10, 10);
			cairo_stroke(cr);
		}
	}

	Window::Control Window::getControlAt(const fenster::Point& position) const
	{
		if(crossBounds.contains(position)) return Control::Close;
		if(maximizeButtonVisible && resizable && fullscreenBounds.contains(position)) return Control::Fullscreen;
		if(minimizeButtonVisible && showInTaskbar && minimizeBounds.contains(position)) return Control::Minimize;
		return Control::None;
	}

	fenster::Rectangle Window::getFullscreenBounds()
	{
		auto parent = getParent();
		if(!parent) return getBounds();
		auto size = parent->getBounds();
		fenster::Rectangle area{0, 0, size.width, size.height};
		if(auto screen = dynamic_cast<Screen*>(parent)) area = screen->getWindowArea();
		// Keep shadows outside the usable area; the dock paints above the bottom shadow.
		return {area.x - shadowSize, area.y - shadowSize, area.width + 2 * shadowSize,
		        area.height + 2 * shadowSize};
	}

	void Window::toggleFullscreen()
	{
		if(fullscreen)
		{
			fullscreen = false;
			setBounds(restoreBounds);
		}
		else if(resizable && getParent())
		{
			auto area = getFullscreenBounds();
			if(area.width <= 2 * shadowSize || area.height <= 2 * shadowSize) return;
			restoreBounds = getBounds();
			fullscreen = true;
			setBounds(area);
		}
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void Window::minimize()
	{
		setVisible(false);
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
			if(fullscreen)
			{
				auto delta = me.screenPosition - pressScreenPoint;
				if(std::abs(delta.x) < 4 && std::abs(delta.y) < 4) return;
				// Keep the same horizontal title-bar anchor under the pointer.
				pressPoint.x = std::clamp(int(int64_t(pressPoint.x) * restoreBounds.width / pressBounds.width),
				                          shadowSize, restoreBounds.width - shadowSize);
				pressBounds = restoreBounds;
				fullscreen = false;
			}
			fenster::Point newLocation = me.screenPosition - pressPoint;
			if(getParent()) newLocation = newLocation - getParent()->getLocationOnScreen();
			setBounds({newLocation.x, newLocation.y, pressBounds.width, pressBounds.height});
			Cursor::set("drag");
			return;
		}

		fenster::Point newLocation = me.screenPosition - pressPoint;
		if(getParent()) newLocation = newLocation - getParent()->getLocationOnScreen();

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

	WindowResizeMode Window::getResizeModeAt(const fenster::Point& pos) const
	{
		auto currentBounds = getBounds();
		WindowResizeMode resizeMode = RESIZE_MODE_NONE;

		if(resizable && !fullscreen)
		{
			if((pos.x < shadowSize + cornerSize / 2) && (pos.x > shadowSize - cornerSize / 2) && (
				   pos.y < cornerSize) && (pos.y > shadowSize - cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_TOP | RESIZE_MODE_LEFT;
			}
			else if((pos.x > currentBounds.width - shadowSize - cornerSize / 2) && (
				        pos.x < currentBounds.width - shadowSize + cornerSize / 2) && (
				        pos.y < cornerSize) && (pos.y > shadowSize - cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_TOP | RESIZE_MODE_RIGHT;
			}
			else if((pos.x < shadowSize + cornerSize / 2) && (pos.x > shadowSize - cornerSize / 2)
			        &&
			        (pos.y > currentBounds.height - shadowSize - cornerSize / 2) && (
				        pos.y < currentBounds.height - shadowSize + cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_BOTTOM | RESIZE_MODE_LEFT;
			}
			else if((pos.x > currentBounds.width - shadowSize - cornerSize / 2) && (
				        pos.x < currentBounds.width - shadowSize + cornerSize / 2) && (
				        pos.y > currentBounds.height - shadowSize - cornerSize / 2) && (
				        pos.y < currentBounds.height - shadowSize + cornerSize / 2))
			{
				resizeMode |= RESIZE_MODE_BOTTOM | RESIZE_MODE_RIGHT;
			}
			else if(pos.y < shadowSize + borderWidth / 2 && pos.y > shadowSize - borderWidth / 2)
			{
				resizeMode = RESIZE_MODE_TOP;
			}
			else if(pos.x < shadowSize + borderWidth / 2 && pos.x > shadowSize - borderWidth / 2)
			{
				resizeMode = RESIZE_MODE_LEFT;
			}
			else if((pos.y > currentBounds.height - shadowSize - borderWidth / 2) && (
				        pos.y < currentBounds.height - shadowSize + borderWidth / 2))
			{
				resizeMode = RESIZE_MODE_BOTTOM;
			}
			else if((pos.x > currentBounds.width - shadowSize - borderWidth / 2) && (
				        pos.x < currentBounds.width - shadowSize + borderWidth / 2))
			{
				resizeMode = RESIZE_MODE_RIGHT;
			}
		}

		return resizeMode;
	}

	std::string Window::getCursorAt(const fenster::Point& position) const
	{
		switch(getResizeModeAt(position))
		{
			case RESIZE_MODE_TOP | RESIZE_MODE_LEFT:
			case RESIZE_MODE_BOTTOM | RESIZE_MODE_RIGHT: return "resize-nwes";
			case RESIZE_MODE_TOP | RESIZE_MODE_RIGHT:
			case RESIZE_MODE_BOTTOM | RESIZE_MODE_LEFT: return "resize-nesw";
			case RESIZE_MODE_TOP:
			case RESIZE_MODE_BOTTOM: return "resize-ns";
			case RESIZE_MODE_LEFT:
			case RESIZE_MODE_RIGHT: return "resize-ew";
			default: return "default";
		}
	}

	void Window::startDragOrResize(MouseEvent& me)
	{
		pressPoint = me.position;
		pressScreenPoint = me.screenPosition;
		pressBounds = getBounds();
		resizeMode = getResizeModeAt(pressPoint);

		if(resizeMode == RESIZE_MODE_NONE)
		{
			if(pressPoint.y >= shadowSize && pressPoint.y < shadowSize + titleHeight &&
			   !(minimizeButtonVisible && minimizeBounds.contains(pressPoint)) &&
			   !(maximizeButtonVisible && fullscreenBounds.contains(pressPoint)) &&
			   !crossBounds.contains(pressPoint))
			{
				resizeMode = RESIZE_MODE_MOVE;
				Cursor::set("drag");
			}
		}
	}

	Component* Window::handleMouseEvent(MouseEvent& me)
	{
		auto updateHover = [&](Control control)
		{
			if(hoveredControl != control)
			{
				hoveredControl = control;
				markFor(COMPONENT_REQUIREMENT_PAINT);
			}
		};

		if(me.type == fenster::MouseEventType::Drag)
		{
			if(pressedControl != Control::None) updateHover(getControlAt(me.position));
			else if(resizeMode != RESIZE_MODE_NONE) dragOrResize(me);
			return this;
		}
		if(me.type == fenster::MouseEventType::Release || me.type == fenster::MouseEventType::DragRelease)
		{
			const auto released = pressedControl;
			pressedControl = Control::None;
			resizeMode = RESIZE_MODE_NONE;
			markFor(COMPONENT_REQUIREMENT_PAINT);
			if(me.type == fenster::MouseEventType::Release && !(me.buttons & fenster::MouseButton::Button1) &&
			   released != Control::None && released == getControlAt(me.position))
			{
				updateHover(Control::None);
				if(released == Control::Close) close();
				else if(released == Control::Minimize) minimize();
				else toggleFullscreen();
			}
			return this;
		}

		Component* handledByChild = Component::handleMouseEvent(me);
		if(handledByChild)
		{
			updateHover(Control::None);
			return handledByChild;
		}
		if(me.type == fenster::MouseEventType::Move || me.type == fenster::MouseEventType::Enter)
			updateHover(getControlAt(me.position));
		else if(me.type == fenster::MouseEventType::Press && (me.buttons & fenster::MouseButton::Button1))
		{
			pressedControl = getControlAt(me.position);
			updateHover(pressedControl);
			markFor(COMPONENT_REQUIREMENT_PAINT);
			if(pressedControl == Control::None) startDragOrResize(me);
		}
		else if(me.type == fenster::MouseEventType::Leave)
			updateHover(Control::None);
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
		if(property == fenster::ComponentProperty::MinimizeButtonVisible) { *out = minimizeButtonVisible; return true; }
		if(property == fenster::ComponentProperty::MaximizeButtonVisible) { *out = maximizeButtonVisible; return true; }
		if(property == fenster::ComponentProperty::ShowInTaskbar) { *out = showInTaskbar; return true; }
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
		if(property == fenster::ComponentProperty::MinimizeButtonVisible ||
		   property == fenster::ComponentProperty::MaximizeButtonVisible)
		{
			auto& visible = property == fenster::ComponentProperty::MinimizeButtonVisible ?
			                minimizeButtonVisible : maximizeButtonVisible;
			if(visible != bool(value))
			{
				visible = value;
				pressedControl = hoveredControl = Control::None;
				markFor(COMPONENT_REQUIREMENT_LAYOUT | COMPONENT_REQUIREMENT_PAINT);
			}
			return true;
		}
		if(property == fenster::ComponentProperty::ShowInTaskbar)
		{
			if(showInTaskbar != bool(value)) {
				showInTaskbar = value;
				markFor(COMPONENT_REQUIREMENT_PAINT);
				if(auto screen = dynamic_cast<Screen*>(getParent()))
					screen->callForListeners(fenster::ComponentEventType::Window, [this, screen](EventListenerInfo& info) {
						screen->sendWindowEvent(info.component_id, this, info.target_thread, true);
					});
			}
			return true;
		}
		if(property == fenster::ComponentProperty::Resizable)
		{
			if(!value && fullscreen) toggleFullscreen();
			resizable = value;
			markFor(COMPONENT_REQUIREMENT_PAINT);
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

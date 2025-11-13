// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/scrollpane.hpp"

#include "../../../platform/macos-sdl-sim/sysroot/system/include/libfenster/listener/key_listener.hpp"

#define SCROLLBAR_SIZE 15

namespace fensterserver
{
	ScrollPane::ScrollPane()
	{
		addChild(&horizontalScrollbar, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		horizontalScrollbar.setZIndex(1000);
		horizontalScrollbar.setScrollHandler(this);

		addChild(&verticalScrollbar, COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL);
		verticalScrollbar.setZIndex(1001);
		verticalScrollbar.setScrollHandler(this);
	}


	void ScrollPane::setContent(Component* component)
	{
		if(component == this->content)
		{
			markFor(COMPONENT_REQUIREMENT_LAYOUT);
		}
		else
		{
			if(this->content)
				removeChild(this->content);

			this->content = component;
			addChild(this->content);
		}
	}

	Component *ScrollPane::handleKeyEvent(KeyEvent &event) {
		if (event.info.key == "KEY_ALT_L") {
			keyHorizontalScrollDown = event.info.pressed;
		}
		return nullptr;
	}


	Component* ScrollPane::handleMouseEvent(MouseEvent& event)
	{
		if(event.type == fenster::MouseEventType::Scroll)
		{
			if (keyHorizontalScrollDown) {
				horizontalScrollbar.setModelPosition(horizontalScrollbar.getModelPosition() + event.scroll * 50);
				handleScroll(&horizontalScrollbar);
			}else {
				verticalScrollbar.setModelPosition(verticalScrollbar.getModelPosition() + event.scroll * 50);
				handleScroll(&verticalScrollbar);
			}
			return this;
		}
		return Component::handleMouseEvent(event);
	}

	void ScrollPane::handleScroll(Scrollbar* bar)
	{
		if(bar == &verticalScrollbar)
		{
			scrollPosition.y = -verticalScrollbar.getModelPosition();
			updateContent();
		}
		else if(bar == &horizontalScrollbar)
		{
			scrollPosition.x = -horizontalScrollbar.getModelPosition();
			updateContent();
		}
	}

	fenster::Dimension ScrollPane::calculateViewport(fenster::Dimension contentSize)
	{
		showVbar = false;
		showHbar = false;

		fenster::Dimension viewportSize = getBounds().getSize();

		if(contentSize.height > viewportSize.height)
		{
			showVbar = true;
		}
		if(contentSize.width > (showVbar ? viewportSize.width - SCROLLBAR_SIZE : viewportSize.width))
		{
			showHbar = true;
		}
		if(contentSize.height > (showHbar ? viewportSize.height - SCROLLBAR_SIZE : viewportSize.height))
		{
			showVbar = true;
		}
		if(contentSize.width > (showVbar ? viewportSize.width - SCROLLBAR_SIZE : viewportSize.width))
		{
			showHbar = true;
		}

		if(showHbar)
		{
			viewportSize.height -= SCROLLBAR_SIZE;
		}
		if(showVbar)
		{
			viewportSize.width -= SCROLLBAR_SIZE;
		}
		return viewportSize;
	}

	void ScrollPane::layout()
	{
		if(!content)
			return;

		auto contentSize = content->getEffectivePreferredSize();
		if(fixedWidth)
			contentSize.width = getBounds().width - SCROLLBAR_SIZE;
		if(fixedHeight)
			contentSize.height = getBounds().height - SCROLLBAR_SIZE;
		auto viewportSize = calculateViewport(contentSize);

		auto bounds = getBounds();
		if(showVbar)
		{
			verticalScrollbar.setViewLengths(viewportSize.height, contentSize.height);
			verticalScrollbar.setBounds(
					fenster::Rectangle(bounds.width - SCROLLBAR_SIZE, 0, SCROLLBAR_SIZE, viewportSize.height));
			verticalScrollbar.setVisible(true);
		}
		else
		{
			verticalScrollbar.setVisible(false);
		}

		if(showHbar)
		{
			horizontalScrollbar.setViewLengths(viewportSize.width, contentSize.width);
			horizontalScrollbar.setBounds(
					fenster::Rectangle(0, bounds.height - SCROLLBAR_SIZE, viewportSize.width, SCROLLBAR_SIZE));
			horizontalScrollbar.setVisible(true);
		}
		else
		{
			horizontalScrollbar.setVisible(false);
		}

		updateContent();

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	void ScrollPane::updateContent()
	{
		if(!content)
			return;

		auto contentSize = content->getEffectivePreferredSize();
		if(fixedWidth)
			contentSize.width = getBounds().width - SCROLLBAR_SIZE;
		if(fixedHeight)
			contentSize.height = getBounds().height - SCROLLBAR_SIZE;

		auto viewportSize = calculateViewport(contentSize);

		if(scrollPosition.x > 0)
		{
			scrollPosition.x = 0;
		}
		else if(contentSize.width < viewportSize.width)
		{
			scrollPosition.x = 0;
		}
		else if(scrollPosition.x + contentSize.width < viewportSize.width)
		{
			scrollPosition.x = viewportSize.width - contentSize.width;
		}

		if(scrollPosition.y > 0)
		{
			scrollPosition.y = 0;
		}
		else if(contentSize.height < viewportSize.height)
		{
			scrollPosition.y = 0;
		}
		else if(scrollPosition.y + contentSize.height < viewportSize.height)
		{
			scrollPosition.y = viewportSize.height - contentSize.height;
		}

		content->setBounds(fenster::Rectangle(scrollPosition.x, scrollPosition.y,
		                             fixedWidth ? viewportSize.width : contentSize.width,
		                             fixedHeight ? viewportSize.height : contentSize.height));
	}
}

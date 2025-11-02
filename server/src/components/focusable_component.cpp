// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/focusable_component.hpp"
#include "components/component.hpp"

#include <libfenster/properties.hpp>

namespace fensterserver
{
	Component* FocusableComponent::setFocused(bool focused)
	{
		if(isFocusable())
		{
			setFocusedInternal(focused);

			self->callForListeners(FENSTER_COMPONENT_EVENT_TYPE_FOCUS, [focused](EventListenerInfo& info)
			{
				fenster::ComponentFocusEvent event;
				event.header.type = FENSTER_COMPONENT_EVENT_TYPE_FOCUS;
				event.header.component_id = info.component_id;
				event.now_focused = focused;
				fenster::platformSendMessage(info.target_thread, &event, sizeof(fenster::ComponentFocusEvent),
				                             SYS_TX_NONE);
			});

			return self;
		}

		if(this->dispatchesFocus)
		{
			auto parent = self->getParent();
			if(parent)
				return parent->setFocused(focused);
		}
		return nullptr;
	}


	bool FocusableComponent::setNumericProperty(int property, uint32_t value)
	{
		if(property == FENSTER_UI_PROPERTY_FOCUSABLE)
		{
			this->focusable = value == 1;
			return true;
		}
		if(property == FENSTER_UI_PROPERTY_DISPATCHES_FOCUS)
		{
			this->dispatchesFocus = value == 1;
			return true;
		}
		return false;
	}

	bool FocusableComponent::getNumericProperty(int property, uint32_t* out)
	{
		if(property == FENSTER_UI_PROPERTY_FOCUSABLE)
		{
			*out = this->focusable ? 1 : 0;
			return true;
		}
		if(property == FENSTER_UI_PROPERTY_DISPATCHES_FOCUS)
		{
			*out = this->dispatchesFocus ? 1 : 0;
			return true;
		}
		return false;
	}
}

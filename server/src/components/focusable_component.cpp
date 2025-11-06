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

			self->callForListeners(fenster::ComponentEventType::Focus, [focused](EventListenerInfo& info)
			{
				fenster::ComponentFocusEvent event;
				event.header.type = fenster::ComponentEventType::Focus;
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


	bool FocusableComponent::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Focusable)
		{
			this->focusable = value == 1;
			return true;
		}
		if(property == fenster::ComponentProperty::DispatchesFocus)
		{
			this->dispatchesFocus = value == 1;
			return true;
		}
		return false;
	}

	bool FocusableComponent::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Focusable)
		{
			*out = this->focusable ? 1 : 0;
			return true;
		}
		if(property == fenster::ComponentProperty::DispatchesFocus)
		{
			*out = this->dispatchesFocus ? 1 : 0;
			return true;
		}
		return false;
	}
}

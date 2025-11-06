// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/component.hpp"
#include "libfenster/checkable_component.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	bool CheckableComponent::setChecked(bool checked)
	{
		return setNumericProperty(ComponentProperty::Checked, checked ? 1 : 0);
	}

	bool CheckableComponent::isChecked()
	{
		uint32_t out;
		getNumericProperty(ComponentProperty::Checked, &out);
		return out == 1;
	}

	void CheckableComponent::addCheckedListener(std::function<void(bool)> func)
	{
		this->addListener(ComponentEventType::Checked, new CheckedListenerDispatcher(std::move(func)));
	}
}

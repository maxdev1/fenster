// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/components/checkbox.hpp"
#include "libfenster/components/component.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	Checkbox* Checkbox::create()
	{
		return createComponent<Checkbox, ComponentType::Checkbox>();
	}

	void Checkbox::setEnabled(bool enabled)
	{
		setNumericProperty(ComponentProperty::Enabled, enabled);
	}

	bool Checkbox::isEnabled()
	{
		uint32_t value;
		getNumericProperty(ComponentProperty::Enabled, &value);
		return value;
	}

}

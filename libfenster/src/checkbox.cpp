// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/checkbox.hpp"
#include "libfenster/component.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	Checkbox* Checkbox::create()
	{
		return createComponent<Checkbox, FENSTER_COMPONENT_TYPE_CHECKBOX>();
	}

	void Checkbox::setEnabled(bool enabled)
	{
		setNumericProperty(FENSTER_UI_PROPERTY_ENABLED, enabled);
	}

	bool Checkbox::isEnabled()
	{
		uint32_t value;
		getNumericProperty(FENSTER_UI_PROPERTY_ENABLED, &value);
		return value;
	}

}

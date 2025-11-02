// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libfenster/titled_component.hpp"
#include "libfenster/listener/title_listener.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	bool TitledComponent::setTitle(std::string title)
	{
		return setStringProperty(FENSTER_UI_PROPERTY_TITLE, title);
	}

	std::string TitledComponent::getTitle()
	{
		std::string out;
		getStringProperty(FENSTER_UI_PROPERTY_TITLE, out);
		return out;
	}

	void TitledComponent::addTitleListener(std::function<void(std::string)> func)
	{
		this->addListener(FENSTER_COMPONENT_EVENT_TYPE_TITLE, new TitleListenerDispatcher(std::move(func)));
	}
}

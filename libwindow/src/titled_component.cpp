// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <utility>

#include "libwindow/titled_component.hpp"
#include "libwindow/listener/title_listener.hpp"
#include "libwindow/properties.hpp"

bool g_titled_component::setTitle(std::string title)
{
	return setStringProperty(G_UI_PROPERTY_TITLE, title);
}

std::string g_titled_component::getTitle()
{
	std::string out;
	getStringProperty(G_UI_PROPERTY_TITLE, out);
	return out;
}

void g_titled_component::addTitleListener(std::function<void(std::string)> func)
{
	this->addListener(G_UI_COMPONENT_EVENT_TYPE_TITLE, new g_title_listener_dispatcher(std::move(func)));
}

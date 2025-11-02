// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/image.hpp"
#include "libwindow/properties.hpp"

g_image* g_image::create()
{
	return createComponent<g_image, G_UI_COMPONENT_TYPE_IMAGE>();
}

bool g_image::loadImage(std::string path)
{
	return setStringProperty(G_UI_PROPERTY_IMAGE_SOURCE, path);
}

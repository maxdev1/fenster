// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/image.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	Image* Image::create()
	{
		return createComponent<Image, FENSTER_COMPONENT_TYPE_IMAGE>();
	}

	bool Image::loadImage(std::string path)
	{
		return setStringProperty(ComponentProperty::ImageSource, path);
	}
}

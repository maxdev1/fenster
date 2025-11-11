// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/components/image.hpp"
#include "libfenster/properties.hpp"

namespace fenster
{
	Image* Image::create()
	{
		return createComponent<Image, ComponentType::Image>();
	}

	bool Image::loadImage(std::string path)
	{
		return setStringProperty(ComponentProperty::ImageSource, path);
	}
}

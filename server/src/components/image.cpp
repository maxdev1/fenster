// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/image.hpp"
#include <libfenster/font/font_loader.hpp>
#include <libfenster/properties.hpp>

#include <cairo/cairo.h>
#include <sstream>

namespace fensterserver
{
	Image::Image()
	{
	}

	void Image::loadImage(std::string path)
	{
		fenster::platformAcquireMutex(this->lock);
		if(image)
		{
			cairo_surface_destroy(image);
			image = nullptr;
		}

		image = cairo_image_surface_create_from_png(path.c_str());
		if(cairo_surface_status(image) != CAIRO_STATUS_SUCCESS)
		{
			fenster::platformLog("failed to load image: %s", path.c_str());
			image = nullptr;
		}
		fenster::platformReleaseMutex(this->lock);

		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Image::setStringProperty(fenster::ComponentProperty property, std::string text)
	{
		if(property == fenster::ComponentProperty::ImageSource)
		{
			this->loadImage(text);
			return true;
		}
		return false;
	}

	void Image::paint()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		fenster::platformAcquireMutex(this->lock);

		cairo_save(cr);
		int imageWidth = cairo_image_surface_get_width(image);
		int imageX = bounds.width / 2 - imageWidth / 2;
		int imageY = 10;
		cairo_set_source_surface(cr, image, imageX, imageY);
		cairo_rectangle(cr, imageX, imageY, bounds.width, bounds.height);
		cairo_fill(cr);
		cairo_restore(cr);

		fenster::platformReleaseMutex(this->lock);

		graphics.releaseContext();

	}
}

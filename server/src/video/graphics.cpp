// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "video/graphics.hpp"

#include <stdio.h>
#include <algorithm>
#include <server.hpp>

namespace fensterserver
{
	Graphics::Graphics(uint16_t width, uint16_t height) :
		width(0), height(0)
	{
		resize(width, height);
	}

	Graphics::~Graphics()
	{
		if(context)
			cairo_destroy(context);
		if(surface)
			cairo_surface_destroy(surface);
		fenster::platformDestroyMutex(lock);
	}


	cairo_t* Graphics::acquireContext()
	{
		fenster::platformAcquireMutex(lock);
		if(!surface)
		{
			fenster::platformReleaseMutex(lock);
			return nullptr;
		}

		if(!context)
		{
			context = cairo_create(surface);
			auto contextStatus = cairo_status(context);
			if(contextStatus != CAIRO_STATUS_SUCCESS)
			{
				context = nullptr;
				fenster::platformReleaseMutex(lock);
				return nullptr;
			}
		}

		contextRefCount++;
		return context;
	}

	void Graphics::releaseContext()
	{
		--contextRefCount;
		if(contextRefCount < 0)
		{
			fenster::platformLog("error: over-deref of canvas by %i references", contextRefCount);
			contextRefCount = 0;
			return;
		}

		if(contextRefCount == 0 && context)
		{
			cairo_destroy(context);
			context = nullptr;
		}

		fenster::platformReleaseMutex(lock);
	}

	void Graphics::resize(int newWidth, int newHeight, bool averaged)
	{
		if(newWidth <= 0 || newHeight <= 0)
			return;

		fenster::platformAcquireMutex(lock);
		// Capacity grows independently on each axis; list/grid changes must not
		// discard the retained height when growing width (or vice versa).
		if(averaged)
		{
			newWidth = (newWidth + averageFactor - 1) / averageFactor * averageFactor;
			newHeight = (newHeight + averageFactor - 1) / averageFactor * averageFactor;
		}
		newWidth = std::max(newWidth, int(width));
		newHeight = std::max(newHeight, int(height));
		if(surface && newWidth == width && newHeight == height)
		{
			fenster::platformReleaseMutex(lock);
			return;
		}
		if(surface)
		{
			cairo_surface_destroy(surface);
			surface = nullptr;
		}

		width = newWidth;
		height = newHeight;
		surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
		auto surfaceStatus = cairo_surface_status(surface);
		if(surfaceStatus != CAIRO_STATUS_SUCCESS)
		{
			fenster::platformLog("failed to create graphics surface of size %i %i: %i", width, height, surfaceStatus);
			surface = nullptr;
		}

		fenster::platformReleaseMutex(lock);
	}

	void Graphics::blitTo(Graphics* target, const fenster::Rectangle& clip, const fenster::Point& position)
	{
		fenster::platformAcquireMutex(lock);

		auto cr = target->acquireContext();
		if(cr)
		{
			if(surface)
			{
				auto surfaceStatus = cairo_surface_status(surface);
				if(surfaceStatus != CAIRO_STATUS_SUCCESS)
				{
					fenster::platformLog("bad surface status");
				}
				else
				{
					cairo_save(cr);
					cairo_rectangle(cr, clip.x, clip.y, clip.width, clip.height);
					cairo_clip(cr);
					cairo_set_source_surface(cr, surface, position.x, position.y);
					cairo_paint(cr);
					cairo_restore(cr);
				}
			}

			if(Server::isDebug())
			{
				cairo_save(cr);
				cairo_set_line_width(cr, 2);
				cairo_rectangle(cr, clip.x, clip.y, clip.width, clip.height);
				cairo_set_source_rgba(cr, 1, 0, 0, 1);
				cairo_stroke(cr);
				cairo_restore(cr);
			}

			target->releaseContext();
		}

		fenster::platformReleaseMutex(lock);
	}
}
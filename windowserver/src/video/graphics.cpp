// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "video/graphics.hpp"

#include <stdio.h>
#include <windowserver.hpp>

graphics_t::graphics_t(uint16_t width, uint16_t height) :
	width(width), height(height)
{
	resize(width, height);
}


cairo_t* graphics_t::acquireContext()
{
	platformAcquireMutex(lock);
	if(!surface)
	{
		platformReleaseMutex(lock);
		return nullptr;
	}

	if(!context)
	{
		context = cairo_create(surface);
		auto contextStatus = cairo_status(context);
		if(contextStatus != CAIRO_STATUS_SUCCESS)
		{
			context = nullptr;
			platformReleaseMutex(lock);
			return nullptr;
		}
	}

	contextRefCount++;
	return context;
}

void graphics_t::releaseContext()
{
	--contextRefCount;
	if(contextRefCount < 0)
	{
		platformLog("error: over-deref of canvas by %i references", contextRefCount);
		contextRefCount = 0;
		return;
	}

	if(contextRefCount == 0 && context)
	{
		cairo_destroy(context);
		context = nullptr;
	}

	platformReleaseMutex(lock);
}

void graphics_t::resize(int newWidth, int newHeight, bool averaged)
{
	if(newWidth <= 0 || newHeight <= 0)
		return;

	if(averaged)
	{
		newWidth = newWidth + (averageFactor - newWidth % averageFactor);
		newHeight = newHeight + (averageFactor - newHeight % averageFactor);
	}

	// TODO: Like this, buffers never downscale. Check if we want this:
	if(newWidth <= width && newHeight <= height)
	{
		return;
	}

	platformAcquireMutex(lock);
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
		platformLog("failed to create graphics surface of size %i %i: %i", width, height, surfaceStatus);
		surface = nullptr;
	}

	platformReleaseMutex(lock);
}

void graphics_t::blitTo(graphics_t* target, const g_rectangle& clip, const g_point& position)
{
	platformAcquireMutex(lock);

	auto cr = target->acquireContext();
	if(cr)
	{
		if(surface)
		{
			auto surfaceStatus = cairo_surface_status(surface);
			if(surfaceStatus != CAIRO_STATUS_SUCCESS)
			{
				platformLog("bad surface status");
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

		if(windowserver_t::isDebug())
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

	platformReleaseMutex(lock);
}

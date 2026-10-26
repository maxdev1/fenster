// fenster — lightweight window server and UI toolkit
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "video/image_cache.hpp"

namespace fensterserver
{
	ImageCache::ImageCache(size_t maxBytes, size_t maxEntries) : maxBytes(maxBytes), maxEntries(maxEntries)
	{
	}

	ImageCache::~ImageCache()
	{
		for(auto& entry : entries)
			cairo_surface_destroy(entry.surface);
		fenster::platformDestroyMutex(lock);
	}

	cairo_surface_t* ImageCache::acquire(const std::string& path)
	{
		fenster::platformAcquireMutex(lock);
		for(auto it = entries.begin(); it != entries.end(); ++it)
		{
			if(it->path == path)
			{
				auto surface = cairo_surface_reference(it->surface);
				entries.splice(entries.begin(), entries, it);
				fenster::platformReleaseMutex(lock);
				return surface;
			}
		}
		auto surface = cairo_image_surface_create_from_png(path.c_str());
		if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
		{
			cairo_surface_destroy(surface);
			fenster::platformReleaseMutex(lock);
			return nullptr;
		}
		const auto bytes = static_cast<size_t>(cairo_image_surface_get_stride(surface)) *
		                   static_cast<size_t>(cairo_image_surface_get_height(surface));
		if(maxEntries && bytes <= maxBytes)
		{
			while(!entries.empty() && (entries.size() >= maxEntries || usedBytes > maxBytes - bytes))
			{
				usedBytes -= entries.back().bytes;
				cairo_surface_destroy(entries.back().surface);
				entries.pop_back();
			}
			entries.push_front({path, cairo_surface_reference(surface), bytes});
			usedBytes += bytes;
		}
		fenster::platformReleaseMutex(lock);
		return surface;
	}
}

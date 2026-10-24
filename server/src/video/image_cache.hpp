// fenster — lightweight window server and UI toolkit
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_IMAGE_CACHE
#define FENSTER_SERVER_IMAGE_CACHE

#include <cairo/cairo.h>
#include <libfenster/platform/platform.hpp>
#include <list>
#include <string>

namespace fensterserver
{
	class ImageCache
	{
		struct Entry
		{
			std::string path;
			cairo_surface_t* surface;
			size_t bytes;
		};
		std::list<Entry> entries;
		SYS_MUTEX_T lock = fenster::platformInitializeMutex(false);
		size_t usedBytes = 0;
		size_t maxBytes;
		size_t maxEntries;

	public:
		explicit ImageCache(size_t maxBytes = 4 * 1024 * 1024, size_t maxEntries = 32);
		~ImageCache();
		ImageCache(const ImageCache&) = delete;
		ImageCache& operator=(const ImageCache&) = delete;

		// Returns an owned reference. Eviction cannot invalidate displayed images.
		// Failed reads are not cached, so missing/repaired files can be retried.
		cairo_surface_t* acquire(const std::string& path);
	};
}

#endif

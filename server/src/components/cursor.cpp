// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/cursor.hpp"
#include "server.hpp"

#include <libproperties/properties.hpp>
#include <sstream>

namespace fensterserver
{
	static std::map<std::string, CursorConfiguration> cursorConfigurations;
	static CursorConfiguration* currentConfiguration = 0;

	SYS_MUTEX_T Cursor::inputLock = fenster::platformInitializeMutex(false);
	SYS_MUTEX_T Cursor::configurationLock = fenster::platformInitializeMutex(true);
	bool Cursor::hidden = false;

	void Cursor::setHidden(bool value)
	{
		fenster::platformAcquireMutex(configurationLock);
		hidden = value;
		fenster::platformReleaseMutex(configurationLock);
		Server::instance()->requestPointerUpdate();
	}

	fenster::Point Cursor::position;
	fenster::Point Cursor::nextPosition;

	fenster::MouseButton Cursor::pressedButtons = fenster::MouseButton::None;
	fenster::MouseButton Cursor::nextPressedButtons = fenster::MouseButton::None;


	fenster::ComponentId Cursor::pressedComponent = -1;
	fenster::ComponentId Cursor::draggedComponent = -1;
	fenster::ComponentId Cursor::hoveredComponent = -1;
	fenster::ComponentId Cursor::focusedComponent = -1;

	void Cursor::set(std::string name)
	{
		fenster::platformAcquireMutex(configurationLock);
		bool found = true;
		auto previousConfiguration = currentConfiguration;

		if(cursorConfigurations.count(name) > 0)
		{
			currentConfiguration = &cursorConfigurations[name];
		}
		else if(cursorConfigurations.count("default") > 0)
		{
			currentConfiguration = &cursorConfigurations["default"];
		}
		else
		{
			found = false;
		}
		const bool changed = currentConfiguration != previousConfiguration;
		fenster::platformReleaseMutex(configurationLock);
		if(!found)
			fenster::platformLog("could neither load '%s' cursor nor 'default' cursor", name.c_str());

		if(changed)
			Server::instance()->requestPointerUpdate();
	}

	std::string Cursor::get()
	{
		fenster::platformAcquireMutex(configurationLock);
		auto name = currentConfiguration ? currentConfiguration->name : "default";
		fenster::platformReleaseMutex(configurationLock);
		return name;
	}

	bool Cursor::load(std::string cursorPath)
	{
		// Open config file
		std::string configpath = cursorPath + "/cursor.cfg";
		std::ifstream in(configpath);
		if(!in.good())
		{
			fenster::platformLog("failed to open cursor configuration at %s", configpath.c_str());
			return false;
		}

		properties::Properties props(in);
		auto content = props.getProperties();

		// Read required params
		std::string name = content["name"];
		std::string hitpoint_x = content["hitpoint.x"];
		std::string hitpoint_y = content["hitpoint.y"];
		std::string image = content["image"];

		if(name.empty() || hitpoint_x.empty() || hitpoint_y.empty() || image.empty())
		{
			fenster::platformLog("either name (%s), hitpoint (%s %s), or image (%s) was missing for cursor\n",
			                     name.c_str(), hitpoint_x.c_str(), hitpoint_y.c_str(), image.c_str());
			return false;
		}

		// Convert hitpoint
		std::stringstream stx;
		stx << hitpoint_x;
		int hitpointX;
		stx >> hitpointX;

		std::stringstream sty;
		sty << hitpoint_y;
		int hitpointY;
		sty >> hitpointY;

		std::string cursorImagePath = (cursorPath + "/" + image);

		// check if file exists
		FILE* cursorImageFile = fopen(cursorImagePath.c_str(), "r");
		if(cursorImageFile == NULL)
		{
			fenster::platformLog("failed to open image file at %s\n", cursorImagePath.c_str());
			return false;
		}
		fclose(cursorImageFile);

		// load cursor
		CursorConfiguration pack;
		pack.surface = cairo_image_surface_create_from_png(cursorImagePath.c_str());
		if(cairo_surface_status(pack.surface) != CAIRO_STATUS_SUCCESS)
		{
			fenster::platformLog("failed to load cursor image at '%s' for configuration '%s'", cursorImagePath.c_str(),
			                     cursorPath.c_str());
			cairo_surface_destroy(pack.surface);
			return false;
		}

		pack.hitpoint = fenster::Point(hitpointX, hitpointY);
		pack.size = fenster::Dimension(cairo_image_surface_get_width(pack.surface),
		                               cairo_image_surface_get_height(pack.surface));
		pack.name = name;
		fenster::platformAcquireMutex(configurationLock);
		auto existing = cursorConfigurations.find(name);
		if(existing != cursorConfigurations.end())
			cairo_surface_destroy(existing->second.surface);
		cursorConfigurations[name] = pack;
		fenster::platformReleaseMutex(configurationLock);

		return true;
	}

	void Cursor::paint(Graphics* global, const fenster::Rectangle& clip)
	{
		paintAt(global, position, clip);
	}

	void Cursor::paintAt(Graphics* global, const fenster::Point& position, const fenster::Rectangle& clip)
	{
		auto image = snapshot();
		if(clip.width <= 0 || clip.height <= 0 || !image.getAreaAt(position).intersects(clip))
			return;

		auto cr = global->acquireContext();
		if(!cr)
			return;

		image.paint(global->getSurface(), position, clip);
		global->releaseContext();
	}

	fenster::Rectangle Cursor::getArea()
	{
		return getAreaAt(position);
	}

	fenster::Rectangle Cursor::getAreaAt(const fenster::Point& position)
	{
		return snapshot().getAreaAt(position);
	}

	CursorImage Cursor::snapshot()
	{
		CursorImage image;
		fenster::platformAcquireMutex(configurationLock);
		if(currentConfiguration)
		{
			image.surface = cairo_surface_reference(currentConfiguration->surface);
			image.hitpoint = currentConfiguration->hitpoint;
			image.size = currentConfiguration->size;
			image.visible = !hidden;
		}
		else
		{
			image.size = {FALLBACK_CURSOR_SIZE, FALLBACK_CURSOR_SIZE};
			image.visible = !hidden && !cursorConfigurations.empty();
		}
		fenster::platformReleaseMutex(configurationLock);
		return image;
	}

	fenster::Rectangle CursorImage::getAreaAt(const fenster::Point& position) const
	{
		return {position.x - hitpoint.x, position.y - hitpoint.y, size.width, size.height};
	}

	void CursorImage::paint(cairo_surface_t* target, const fenster::Point& position,
	                        const fenster::Rectangle& requestedClip) const
	{
		if(!visible)
			return;

		const auto area = getAreaAt(position);
		const auto clip = area.clip(requestedClip).clip({0, 0, cairo_image_surface_get_width(target),
		                                                cairo_image_surface_get_height(target)});
		if(clip.width <= 0 || clip.height <= 0)
			return;

		cairo_surface_flush(target);
		auto targetPixels = cairo_image_surface_get_data(target);
		const auto targetStride = cairo_image_surface_get_stride(target);
		const uint8_t* sourcePixels = nullptr;
		int sourceStride = 0;
		bool opaque = false;
		if(surface)
		{
			cairo_surface_flush(surface);
			sourcePixels = cairo_image_surface_get_data(surface);
			sourceStride = cairo_image_surface_get_stride(surface);
			opaque = cairo_image_surface_get_format(surface) == CAIRO_FORMAT_RGB24;
		}

		for(int y = clip.y; y < clip.getBottom(); y++)
		{
			auto targetRow = reinterpret_cast<uint32_t*>(targetPixels + y * targetStride);
			const auto sourceRow = sourcePixels ?
				reinterpret_cast<const uint32_t*>(sourcePixels + (y - area.y) * sourceStride) : nullptr;
			for(int x = clip.x; x < clip.getRight(); x++)
			{
				const uint32_t source = sourceRow ? sourceRow[x - area.x] | (opaque ? 0xFF000000U : 0) : 0xFF000000U;
				const uint32_t alpha = source >> 24;
				if(alpha == 255)
					targetRow[x] = source;
				else if(alpha != 0)
				{
					// Premultiplied OVER, with the same divide-by-255 rounding as pixman.
					const uint32_t inverse = 255 - alpha;
					const uint32_t target = targetRow[x];
					uint32_t rb = (target & 0x00FF00FFU) * inverse + 0x00800080U;
					uint32_t ag = ((target >> 8) & 0x00FF00FFU) * inverse + 0x00800080U;
					rb = ((rb + ((rb >> 8) & 0x00FF00FFU)) >> 8) & 0x00FF00FFU;
					ag = ((ag + ((ag >> 8) & 0x00FF00FFU)) >> 8) & 0x00FF00FFU;
					targetRow[x] = source + rb + (ag << 8);
				}
			}
		}
		cairo_surface_mark_dirty_rectangle(target, clip.x, clip.y, clip.width, clip.height);
	}
}

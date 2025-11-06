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

	fenster::Point Cursor::position;
	fenster::Point Cursor::nextPosition;

	fenster::MouseButton Cursor::pressedButtons = fenster::MouseButton::None;
	fenster::MouseButton Cursor::nextPressedButtons = fenster::MouseButton::None;

	int Cursor::nextScroll = 0;

	fenster::ComponentId Cursor::pressedComponent = -1;
	fenster::ComponentId Cursor::draggedComponent = -1;
	fenster::ComponentId Cursor::hoveredComponent = -1;
	fenster::ComponentId Cursor::focusedComponent = -1;

	void Cursor::set(std::string name)
	{
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
			fenster::platformLog("could neither load '%s' cursor nor 'default' cursor", name.c_str());
		}

		Screen* screen = Server::instance()->screen;
		if(screen)
		{
			screen->markDirty(getArea());
		}
	}

	std::string Cursor::get()
	{
		if(currentConfiguration)
			return currentConfiguration->name;
		return "default";
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
		if(pack.surface == nullptr)
		{
			fenster::platformLog("failed to load cursor image at '%s' for configuration '%s'", cursorImagePath.c_str(),
			                     cursorPath.c_str());
			return false;
		}

		pack.hitpoint = fenster::Point(hitpointX, hitpointY);
		pack.size = fenster::Dimension(cairo_image_surface_get_width(pack.surface),
		                               cairo_image_surface_get_height(pack.surface));
		pack.name = name;
		cursorConfigurations[name] = pack;

		return true;
	}

	void Cursor::paint(Graphics* global)
	{
		auto cr = global->acquireContext();
		if(!cr)
			return;

		cairo_reset_clip(cr);

		if(currentConfiguration)
		{
			cairo_set_source_surface(cr, currentConfiguration->surface, position.x - currentConfiguration->hitpoint.x,
			                         position.y - currentConfiguration->hitpoint.y);
			cairo_paint(cr);
		}
		else if(cursorConfigurations.size() > 0)
		{
			cairo_set_source_rgb(cr, 0, 0, 0);
			cairo_rectangle(cr, position.x, position.y, FALLBACK_CURSOR_SIZE, FALLBACK_CURSOR_SIZE);
			cairo_fill(cr);
		}

		global->releaseContext();
	}

	fenster::Rectangle Cursor::getArea()
	{
		if(currentConfiguration)
		{
			return {position.x - currentConfiguration->hitpoint.x,
			        position.y - currentConfiguration->hitpoint.y,
			        currentConfiguration->size.width,
			        currentConfiguration->size.height};
		}

		return {position.x, position.y, FALLBACK_CURSOR_SIZE, FALLBACK_CURSOR_SIZE};
	}
}

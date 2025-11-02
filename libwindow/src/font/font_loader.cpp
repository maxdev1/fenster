// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/font/font_loader.hpp"
#include "libwindow/font/font_manager.hpp"
#include "libwindow/platform/platform.hpp"

g_font* g_font_loader::getFont(std::string path, std::string name)
{
	g_font* existing = g_font_manager::getInstance()->getFont(name);
	if(existing)
		return existing;

	g_font* newFont = g_font::load(path, name);
	if(!newFont)
		return nullptr;

	if(g_font_manager::getInstance()->registerFont(name, newFont))
		return newFont;

	delete newFont;
	return nullptr;
}

g_font* g_font_loader::getSystemFont(std::string name)
{
	return getFont(platformGetFontPath(name), name);
}

g_font* g_font_loader::get(std::string name)
{
	g_font* font = getSystemFont(name);
	if(font)
		return font;
	return getDefault();
}

g_font* g_font_loader::getDefault()
{
	return getSystemFont("default");
}

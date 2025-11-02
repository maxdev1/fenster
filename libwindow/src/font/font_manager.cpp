// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/font/font_manager.hpp"

static g_font_manager* instance = 0;

g_font_manager* g_font_manager::getInstance()
{
	if(instance == 0)
	{
		instance = new g_font_manager();
	}
	return instance;
}

g_font_manager::g_font_manager()
{
	initializeEngine();
}

g_font_manager::~g_font_manager()
{
	destroyEngine();
}

void g_font_manager::initializeEngine()
{
	FT_Error error = FT_Init_FreeType(&library);
	if(error)
		printf("freetype2 failed at FT_Init_FreeType with error code %i\n", error);
}

void g_font_manager::destroyEngine()
{
	FT_Error error = FT_Done_Library(library);
	if(error)
		printf("freetype2 failed at FT_Done_Library with error code %i\n", error);
}

g_font* g_font_manager::getFont(std::string name)
{
	if(fontRegistry.count(name) > 0)
		return fontRegistry[name];
	return nullptr;
}

bool g_font_manager::registerFont(std::string name, g_font* font)
{
	if(fontRegistry.count(name) > 0)
	{
		printf("tried to create font '%s' that already exists\n", name.c_str());
		return false;
	}

	fontRegistry[name] = font;
	return true;
}

void g_font_manager::destroyFont(g_font* font)
{
	fontRegistry.erase(font->getName());
	delete font;
}

FT_Library g_font_manager::getLibraryHandle()
{
	return library;
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/font/font_manager.hpp"

namespace fenster
{
	static FontManager* instance = 0;

	FontManager* FontManager::getInstance()
	{
		if(instance == 0)
		{
			instance = new FontManager();
		}
		return instance;
	}

	FontManager::FontManager()
	{
		initializeEngine();
	}

	FontManager::~FontManager()
	{
		destroyEngine();
	}

	void FontManager::initializeEngine()
	{
		FT_Error error = FT_Init_FreeType(&library);
		if(error)
			printf("freetype2 failed at FT_Init_FreeType with error code %i\n", error);
	}

	void FontManager::destroyEngine()
	{
		FT_Error error = FT_Done_Library(library);
		if(error)
			printf("freetype2 failed at FT_Done_Library with error code %i\n", error);
	}

	Font* FontManager::getFont(std::string name)
	{
		if(fontRegistry.count(name) > 0)
			return fontRegistry[name];
		return nullptr;
	}

	bool FontManager::registerFont(std::string name, Font* font)
	{
		if(fontRegistry.count(name) > 0)
		{
			printf("tried to create font '%s' that already exists\n", name.c_str());
			return false;
		}

		fontRegistry[name] = font;
		return true;
	}

	void FontManager::destroyFont(Font* font)
	{
		fontRegistry.erase(font->getName());
		delete font;
	}

	FT_Library FontManager::getLibraryHandle()
	{
		return library;
	}
}

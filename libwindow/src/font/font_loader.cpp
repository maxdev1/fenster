// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libwindow/font/font_loader.hpp"
#include "libwindow/font/font_manager.hpp"
#include "libwindow/platform/platform.hpp"

namespace fenster
{
	Font* FontLoader::getFont(std::string path, std::string name)
	{
		Font* existing = FontManager::getInstance()->getFont(name);
		if(existing)
			return existing;

		Font* newFont = Font::load(path, name);
		if(!newFont)
			return nullptr;

		if(FontManager::getInstance()->registerFont(name, newFont))
			return newFont;

		delete newFont;
		return nullptr;
	}

	Font* FontLoader::getSystemFont(std::string name)
	{
		return getFont(platformGetFontPath(name), name);
	}

	Font* FontLoader::get(std::string name)
	{
		Font* font = getSystemFont(name);
		if(font)
			return font;
		return getDefault();
	}

	Font* FontLoader::getDefault()
	{
		return getSystemFont("default");
	}
}

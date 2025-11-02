// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_FONT_FONTMANAGER
#define LIBFENSTER_FONT_FONTMANAGER

#include "libfenster/font/font.hpp"
#include "libfenster/font/freetype.hpp"
#include <map>
#include <string>

namespace fenster
{
	class FontManager
	{
		FT_Library library;
		std::map<std::string, Font*> fontRegistry;

		FontManager();
		~FontManager();

		void initializeEngine();
		void destroyEngine();

	public:
		/**
		 * @return the instance of the font manager singleton
		 */
		static FontManager* getInstance();

		/**
		 * Registers the font.
		 *
		 * @param name			name to which the font shall be registered
		 */
		bool registerFont(std::string name, Font* font);

		/**
		 * Looks for an existing font with the "name".
		 *
		 * @param name	the name to which the font is registered
		 */
		Font* getFont(std::string name);

		/**
		 * Deletes the font and removes it from the font registry.
		 *
		 * @param font	the font to destroy
		 */
		void destroyFont(Font* font);

		/**
		 * @return the freetype library handle
		 */
		FT_Library getLibraryHandle();
	};
}

#endif

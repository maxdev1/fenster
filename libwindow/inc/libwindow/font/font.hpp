// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_FONT_FONT
#define LIBWINDOW_FONT_FONT

#include "libwindow/font/freetype.hpp"

#include <cairo/cairo-ft.h>
#include <cairo/cairo.h>
#include <map>
#include <string>

namespace fenster
{
	enum class FontStyle : uint8_t
	{
		NORMAL,
		ITALIC,
		BOLD
	};

	class Font
	{
		uint8_t* data;
		std::string name;
		FontStyle style = FontStyle::NORMAL;
		bool hint = true;

		FT_Face face;
		cairo_font_face_t* cairo_face = nullptr;

		static bool readAllBytes(FILE* file, uint32_t offset, uint8_t* buffer, uint32_t len);

	public:
		/**
		 * Creates an empty font with the "name". The "source" data
		 * is copied to an internal buffer.
		 *
		 * @param name			font lookup name
		 * @param source		font data
		 * @param sourceLength	font data length
		 */
		Font(std::string name, uint8_t* source, uint32_t sourceLength);

		/**
		 * Loads the font from the given path.
		 *
		 * @param path the file path
		 * @param name the font name
		 * @return either the font, or 0
		 */
		static Font* load(std::string path, std::string name);

		/**
		 * Destroys the font, deleting all associated Glyph
		 * objects and freeing the freetype face.
		 */
		~Font();

		/**
		 * @return whether the font was successfully initialized.
		 */
		bool isValid();

		/**
		 * @return the name of the font
		 */
		std::string getName()
		{
			return name;
		}

		/**
		 *
		 */
		cairo_font_face_t* getFace()
		{
			return cairo_face;
		}
	};
}

#endif

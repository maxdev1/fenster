// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/font/font.hpp"
#include "libfenster/font/font_manager.hpp"
#include <string.h>

namespace fenster
{
	Font::Font(std::string name, uint8_t* source, uint32_t sourceLength) : name(name)
	{
		data = new uint8_t[sourceLength];
		if(!data)
		{
			printf("failed to allocate memory buffer for font '%s'\n", name);
			return;
		}
		memcpy(data, source, sourceLength);

		FT_Error error = FT_New_Memory_Face(FontManager::getInstance()->getLibraryHandle(), data, sourceLength, 0, &face);
		if(error)
		{
			printf("freetype2 failed at FT_New_Memory_Face with error code %i\n", error);
			delete data;
			return;
		}

		cairo_face = cairo_ft_font_face_create_for_ft_face(face, 0);
		cairo_status_t status = cairo_font_face_status(cairo_face);
		if(status != CAIRO_STATUS_SUCCESS)
		{
			FT_Done_Face(face);
			printf("cairo failed at cairo_ft_font_face_create_for_ft_face with error %i\n", status);
			cairo_face = 0;
			delete data;
			return;
		}
	}

	Font::~Font()
	{
		if(cairo_face)
		{
			cairo_font_face_destroy(cairo_face);
			FT_Done_Face(face);
			delete data;
		}
	}

	bool Font::readAllBytes(FILE* file, uint32_t offset, uint8_t* buffer, uint32_t len)
	{
		uint32_t remain = len;
		fseek(file, offset, SEEK_SET);

		while(remain)
		{
			size_t read = fread(&buffer[len - remain], 1, remain, file);
			if(read <= 0)
				return false;
			remain -= read;
		}

		return true;
	}

	Font* Font::load(std::string path, std::string name)
	{
		FILE* file = fopen(path.c_str(), "rb");
		if(!file)
			return nullptr;

		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		if(length == -1)
		{
			fclose(file);
			return nullptr;
		}

		uint8_t* content = new uint8_t[length];
		if(!readAllBytes(file, 0, content, length))
		{
			fclose(file);
			delete content;
			return nullptr;
		}
		fclose(file);

		Font* font = new Font(name, content, length);
		if(!font->isValid())
		{
			delete font;
			return nullptr;
		}
		return font;
	}

	bool Font::isValid()
	{
		return cairo_face != nullptr;
	}
}

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_FONT_FONTLOADER
#define LIBWINDOW_FONT_FONTLOADER

#include "libwindow/font/font.hpp"
#include <string>

namespace fenster
{
	class FontLoader
	{
		static Font* getFont(std::string path, std::string name);
		static Font* getSystemFont(std::string name);

	public:
		static Font* get(std::string name);
		static Font* getDefault();
	};
}

#endif

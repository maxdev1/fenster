// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBFONT_TEXT_FONTLOADER__
#define __LIBFONT_TEXT_FONTLOADER__

#include "libwindow/font/font.hpp"
#include <string>

class g_font_loader
{
  private:
	static g_font* getFont(std::string path, std::string name);
	static g_font* getSystemFont(std::string name);

  public:
	static g_font* get(std::string name);
	static g_font* getDefault();
};

#endif

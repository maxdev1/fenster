// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBFONT_TEXT_FONTMANAGER__
#define __LIBFONT_TEXT_FONTMANAGER__

#include "libwindow/font/font.hpp"
#include "libwindow/font/freetype.hpp"
#include <map>
#include <string>

class g_font_manager
{
  private:
    FT_Library library;
    std::map<std::string, g_font*> fontRegistry;

    g_font_manager();
    ~g_font_manager();

    void initializeEngine();
    void destroyEngine();

  public:
    /**
     * @return the instance of the font manager singleton
     */
    static g_font_manager* getInstance();

    /**
     * Registers the font.
     *
     * @param name			name to which the font shall be registered
     */
    bool registerFont(std::string name, g_font* font);

    /**
     * Looks for an existing font with the "name".
     *
     * @param name	the name to which the font is registered
     */
    g_font* getFont(std::string name);

    /**
     * Deletes the font and removes it from the font registry.
     *
     * @param font	the font to destroy
     */
    void destroyFont(g_font* font);

    /**
     * @return the freetype library handle
     */
    FT_Library getLibraryHandle();
};

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBFONT_TEXT_TEXTLAYOUTER__
#define __LIBFONT_TEXT_TEXTLAYOUTER__

#include "libwindow/metrics/dimension.hpp"
#include "libwindow/metrics/point.hpp"
#include "libwindow/metrics/rectangle.hpp"
#include "libwindow/font/font.hpp"
#include "libwindow/font/text_alignment.hpp"
#include <vector>

/**
 *
 */
struct g_positioned_glyph
{
	g_positioned_glyph() : line(-1), glyph(0), glyph_count(0)
	{
	}

	int line;
	g_point position;

	g_dimension size;
	g_point advance;

	cairo_glyph_t* glyph;
	int glyph_count;
};

/**
 *
 */
struct g_layouted_text
{

	// List of glyphs with their positions
	std::vector<g_positioned_glyph> positions;

	// Bounds of the entire layouted text
	g_rectangle textBounds;

	// Buffers
	cairo_glyph_t* glyph_buffer = nullptr;
	int glyph_count;
	cairo_text_cluster_t* cluster_buffer = nullptr;
	int cluster_count;
};

/**
 *
 */
class g_text_layouter
{
  private:
	/**
	 *
	 */
	g_text_layouter()
	{
	}

  public:
	/**
	 * @return the instance of the font manager singleton
	 */
	static g_text_layouter* getInstance();

	/**
	 *
	 */
	g_layouted_text* initializeBuffer();

	/**
	 *
	 */
	void layout(cairo_t* cr, const char* text, g_font* font, int size, g_rectangle bounds, g_text_alignment alignment, g_layouted_text* layout,
				bool breakOnOverflow = true);

	/**
	 *
	 */
	void destroyLayout(g_layouted_text* layout);
};

#endif

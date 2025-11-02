// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_FONT_TEXTLAYOUTER
#define LIBFENSTER_FONT_TEXTLAYOUTER

#include "libfenster/metrics/dimension.hpp"
#include "libfenster/metrics/point.hpp"
#include "libfenster/metrics/rectangle.hpp"
#include "libfenster/font/font.hpp"
#include "libfenster/font/text_alignment.hpp"
#include <vector>

namespace fenster
{
	/**
	 *
	 */
	struct PositionedGlyph
	{
		PositionedGlyph() :
			line(-1), glyph(0), glyph_count(0)
		{
		}

		int line;
		Point position;

		Dimension size;
		Point advance;

		cairo_glyph_t* glyph;
		int glyph_count;
	};

	/**
	 *
	 */
	struct LayoutedText
	{
		// List of glyphs with their positions
		std::vector<PositionedGlyph> positions;

		// Bounds of the entire layouted text
		Rectangle textBounds;

		// Buffers
		cairo_glyph_t* glyph_buffer = nullptr;
		int glyph_count;
		cairo_text_cluster_t* cluster_buffer = nullptr;
		int cluster_count;
	};

	/**
	 *
	 */
	class TextLayouter
	{
		TextLayouter()
		{
		}

	public:
		/**
		 * @return the instance of the font manager singleton
		 */
		static TextLayouter* getInstance();

		/**
		 *
		 */
		LayoutedText* initializeBuffer();

		/**
		 *
		 */
		void layout(cairo_t* cr, const char* text, Font* font, int size, Rectangle bounds, TextAlignment alignment,
					LayoutedText* layout,
					bool breakOnOverflow = true);

		/**
		 *
		 */
		void destroyLayout(LayoutedText* layout);
	};
}

#endif

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
	struct PositionedGlyph
	{
		int line = -1;
		Point renderPosition;
		Rectangle bounds;
		char controlChar = -1;
		cairo_glyph_t* glyph = nullptr;
		int glyphCount = 0;
	};

	struct LayoutedText
	{
		std::vector<PositionedGlyph> positions;
		Rectangle textBounds;

		cairo_glyph_t* glyphBuffer = nullptr;
		int glyphCount;
		cairo_text_cluster_t* clusterBuffer = nullptr;
		int clusterCount;
	};

	class TextLayouter
	{
	public:
		static LayoutedText* initializeLayout();

		static void layout(
			cairo_t* cr,
			const char* text,
			Font* font,
			int fontSize,
			Rectangle bounds,
			TextAlignment alignment,
			LayoutedText* layout,
			bool breakOnOverflow = true
		);

		static void destroyLayout(LayoutedText* layout);

		static void paintChar(cairo_t* cr, const PositionedGlyph& value, Point offset = Point(0, 0));
	};
}

#endif

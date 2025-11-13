// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/font/text_layouter.hpp"

#include "libfenster/platform/platform.hpp"

#define BOUNDS_EMPTY 0xFFFFFF

namespace fenster
{
	void rightAlign(LayoutedText* text, int line, int lineWidth, Rectangle& bounds)
	{
		int diff = bounds.width - bounds.x - lineWidth;
		for (PositionedGlyph& g: text->positions)
		{
			if (g.line == line)
				g.renderPosition.x += diff;
		}
	}

	void centerAlign(LayoutedText* text, int line, int lineWidth, Rectangle& bounds)
	{
		int diff = (bounds.width - bounds.x) / 2 - lineWidth / 2;
		for (PositionedGlyph& g: text->positions)
		{
			if (g.line == line)
				g.renderPosition.x += diff;
		}
	}

	LayoutedText* TextLayouter::initializeLayout()
	{
		return new LayoutedText();
	}

	void TextLayouter::layout(cairo_t* cr, const char* text, Font* font, int fontSize,
	                          Rectangle bounds, TextAlignment alignment,
	                          LayoutedText* layout, bool breakOnOverflow)
	{
		if (!font)
			return;

		size_t textLength = strlen(text);

		// starting coordinates
		int x = bounds.x;
		int y = bounds.y;
		int lineStartX = x;

		// created the scaled font face
		cairo_set_font_face(cr, font->getFace());
		cairo_set_font_size(cr, fontSize);
		auto scaledFace = cairo_get_scaled_font(cr);

		int line = 0;
		int lineHeight = fontSize + 4;

		// Let cairo generate glyph data
		auto oldGlyphBuffer = layout->glyphBuffer;
		auto oldClusterBuffer = layout->clusterBuffer;
		cairo_text_cluster_flags_t cluster_flags;
		cairo_status_t stat = cairo_scaled_font_text_to_glyphs(scaledFace, 0, 0, text, textLength,
		                                                       &layout->glyphBuffer, &layout->glyphCount,
		                                                       &layout->clusterBuffer, &layout->clusterCount,
		                                                       &cluster_flags);
		if (oldGlyphBuffer != nullptr && layout->glyphBuffer != oldGlyphBuffer)
			cairo_glyph_free(oldGlyphBuffer);
		if (oldClusterBuffer != nullptr && layout->clusterBuffer != oldClusterBuffer)
			cairo_text_cluster_free(oldClusterBuffer);

		// Clear layout positions
		layout->positions.clear();
		if (stat != CAIRO_STATUS_SUCCESS)
			return;

		// positions in bytes and glyphs
		size_t byteOffset = 0;
		size_t glyphOffset = 0;

		// text extents
		cairo_text_extents_t extents;

		bool lastNewLine = false;
		for (int i = 0; i < layout->clusterCount; i++)
		{
			cairo_text_cluster_t* cluster = &layout->clusterBuffer[i];
			cairo_glyph_t* glyphs = &layout->glyphBuffer[glyphOffset];

			PositionedGlyph positioned;
			positioned.glyph = glyphs;
			positioned.glyphCount = cluster->num_glyphs;
			cairo_scaled_font_glyph_extents(scaledFace, positioned.glyph, positioned.glyphCount, &extents);

			// Check for control characters
			bool newline = false;
			char controlChar = -1;
			if (cluster->num_bytes == 1)
			{
				if (text[byteOffset] == '\n')
				{
					newline = true;
					controlChar = text[byteOffset];
				} else if (text[byteOffset] == '\t')
				{
					controlChar = text[byteOffset];
				}
			}

			// Wouldn't match in line or is break character? Start next line
			if (lastNewLine || (breakOnOverflow && (x + extents.width > bounds.width)))
			{
				if (alignment == TextAlignment::RIGHT)
					rightAlign(layout, line, x - lineStartX, bounds);
				else if (alignment == TextAlignment::CENTER)
					centerAlign(layout, line, x - lineStartX, bounds);

				++line;
				x = bounds.x;
				lineStartX = x;
				y += lineHeight;
			}

			positioned.bounds.x = x;
			positioned.bounds.y = y;
			positioned.bounds.width = std::max(extents.width, extents.x_advance);
			positioned.bounds.height = lineHeight;

			positioned.renderPosition.x = x - glyphs->x;
			positioned.renderPosition.y = y + fontSize;
			positioned.line = line;
			positioned.controlChar = controlChar;

			// Add position
			layout->positions.push_back(positioned);

			// Jump to next
			x += extents.x_advance;

			// increase positions
			glyphOffset += cluster->num_glyphs;
			byteOffset += cluster->num_bytes;

			lastNewLine = newline;
		}

		// Align last line
		if (alignment == TextAlignment::RIGHT)
			rightAlign(layout, line, x - lineStartX, bounds);
		else if (alignment == TextAlignment::CENTER)
			centerAlign(layout, line, x - lineStartX, bounds);

		// Set text bounds
		int tbTop = BOUNDS_EMPTY;
		int tbLeft = BOUNDS_EMPTY;
		int tbRight = 0;
		int tbBottom = 0;
		for (PositionedGlyph& p: layout->positions)
		{
			tbLeft = std::min(p.bounds.x, tbLeft);
			tbTop = std::min(p.bounds.y, tbTop);
			tbRight = std::max(tbRight, p.bounds.x + p.bounds.width);
			tbBottom = std::max(tbBottom, p.bounds.y + p.bounds.height);
		}

		if (tbTop != BOUNDS_EMPTY && tbLeft != BOUNDS_EMPTY)
		{
			layout->textBounds.x = tbLeft;
			layout->textBounds.y = tbTop;
			layout->textBounds.width = tbRight;
			layout->textBounds.height = tbBottom;
		}
	}

	void TextLayouter::destroyLayout(LayoutedText* layout)
	{
		if (layout->glyphBuffer)
			free(layout->glyphBuffer);
		if (layout->clusterBuffer)
			free(layout->clusterBuffer);
		delete layout;
	}

	void TextLayouter::paintChar(cairo_t* cr, const PositionedGlyph& g, Point offset)
	{
		cairo_translate(cr, g.renderPosition.x + offset.x, g.renderPosition.y + offset.y);
		cairo_glyph_path(cr, g.glyph, g.glyphCount);
		cairo_fill(cr);
	}
}

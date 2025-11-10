// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/font/text_layouter.hpp"

#define BOUNDS_EMPTY 0xFFFFFF

namespace fenster
{
	static TextLayouter* instance = 0;

	TextLayouter* TextLayouter::getInstance()
	{
		if(!instance)
			instance = new TextLayouter();
		return instance;
	}

	void rightAlign(LayoutedText* text, int line, int lineWidth, Rectangle& bounds)
	{
		int diff = bounds.width - bounds.x - lineWidth;
		for(PositionedGlyph& g : text->positions)
		{
			if(g.line == line)
				g.position.x += diff;
		}
	}

	void centerAlign(LayoutedText* text, int line, int lineWidth, Rectangle& bounds)
	{
		int diff = (bounds.width - bounds.x) / 2 - lineWidth / 2;
		for(PositionedGlyph& g : text->positions)
		{
			if(g.line == line)
				g.position.x += diff;
		}
	}

	LayoutedText* TextLayouter::initializeBuffer()
	{
		return new LayoutedText();
	}

	void TextLayouter::layout(cairo_t* cr, const char* text, Font* font, int size,
								 Rectangle bounds, TextAlignment alignment,
								 LayoutedText* layout, bool breakOnOverflow)
	{
		if(!font)
			return;

		size_t text_len = strlen(text);

		// starting coordinates
		int x = bounds.x;
		int y = bounds.y;
		int lineStartX = x;

		// created the scaled font face
		cairo_set_font_face(cr, font->getFace());
		cairo_set_font_size(cr, size);
		auto scaled_face = cairo_get_scaled_font(cr);

		int line = 0;
		int lineHeight = size + 3;

		// create glyphs for the text
		auto previous_glyph_buffer = layout->glyph_buffer;
		auto previous_cluster_buffer = layout->cluster_buffer;

		cairo_text_cluster_flags_t cluster_flags;
		cairo_status_t stat = cairo_scaled_font_text_to_glyphs(scaled_face, 0, 0, text, text_len, &layout->glyph_buffer, &layout->glyph_count,
															   &layout->cluster_buffer, &layout->cluster_count, &cluster_flags);

		// free old buffer
		if(previous_glyph_buffer != nullptr && layout->glyph_buffer != previous_glyph_buffer)
		{
			cairo_glyph_free(previous_glyph_buffer);
		}
		if(previous_cluster_buffer != nullptr && layout->cluster_buffer != previous_cluster_buffer)
		{
			cairo_text_cluster_free(previous_cluster_buffer);
		}

		// clear layout entries
		layout->positions.clear();

		// perform layouting
		if(stat == CAIRO_STATUS_SUCCESS)
		{
			// positions in bytes and glyphs
			size_t byte_pos = 0;
			size_t glyph_pos = 0;

			// text extents
			cairo_text_extents_t extents;

            bool lastNewLine = false;
			for(int i = 0; i < layout->cluster_count; i++)
			{
				cairo_text_cluster_t* cluster = &layout->cluster_buffer[i];
				cairo_glyph_t* glyphs = &layout->glyph_buffer[glyph_pos];

				// create new position
				PositionedGlyph positioned;
				positioned.glyph = glyphs;
				positioned.glyph_count = cluster->num_glyphs;
				cairo_scaled_font_glyph_extents(scaled_face, positioned.glyph, positioned.glyph_count, &extents);

				positioned.advance.x = extents.x_advance;
				positioned.advance.y = extents.y_advance;
				positioned.size.width = extents.width;
				positioned.size.height = extents.height;

				// check if newline
				bool isNewline = false;
				char controlChar = -1;
				if(cluster->num_bytes == 1) {
                    if(text[byte_pos] == '\n') {
                        isNewline = true;
                        controlChar = text[byte_pos];
                    } else if(text[byte_pos] == '\t') {
                        controlChar = text[byte_pos];
                    }
				}

				// Wouldn't match in line or is break character? Start next line
				if(lastNewLine || (breakOnOverflow && (x + positioned.size.width > bounds.width)))
				{
				    lastNewLine = false;
					if(alignment == TextAlignment::RIGHT)
					{
						rightAlign(layout, line, x - lineStartX, bounds);
					}
					else if(alignment == TextAlignment::CENTER)
					{
						centerAlign(layout, line, x - lineStartX, bounds);
					}

					++line;
					x = bounds.x;
					lineStartX = x;
					y += lineHeight;
				}

				// Position
				positioned.line = line;
				positioned.position.x = x;
				positioned.position.y = y + lineHeight;
				positioned.controlChar = controlChar;

				// Add position
				layout->positions.push_back(positioned);

				// Jump to next
				x += positioned.advance.x;

				// increase positions
				glyph_pos += cluster->num_glyphs;
				byte_pos += cluster->num_bytes;

				lastNewLine = isNewline;
			}
		}

		if(alignment == TextAlignment::RIGHT)
		{
			rightAlign(layout, line, x - lineStartX, bounds);
		}
		else if(alignment == TextAlignment::CENTER)
		{
			centerAlign(layout, line, x - lineStartX, bounds);
		}

		// Set text bounds
		int tbTop = BOUNDS_EMPTY;
		int tbLeft = BOUNDS_EMPTY;
		int tbRight = 0;
		int tbBottom = 0;

		for(PositionedGlyph& p : layout->positions)
		{
			if(p.position.x < tbLeft)
			{
				tbLeft = p.position.x;
			}
			if(p.position.y < tbTop)
			{
				tbTop = p.position.y;
			}

			// get extents again
			int r = p.position.x + p.size.width;
			if(r > tbRight)
			{
				tbRight = r;
			}
			int b = p.position.y + p.size.height;
			if(b > tbBottom)
			{
				tbBottom = b;
			}
		}

		if(tbTop != BOUNDS_EMPTY && tbLeft != BOUNDS_EMPTY)
		{
			layout->textBounds.x = tbLeft;
			layout->textBounds.y = tbTop;
			layout->textBounds.width = tbRight - tbLeft;
			layout->textBounds.height = tbBottom - tbTop;
		}
	}

	void TextLayouter::destroyLayout(LayoutedText* layout)
	{
		if(layout->glyph_buffer)
			free(layout->glyph_buffer);
		if(layout->cluster_buffer)
			free(layout->cluster_buffer);
		delete layout;
	}
}

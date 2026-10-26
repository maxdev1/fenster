// fenster — lightweight window server and UI toolkit
// Copyright (c) 2026 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_ICONS
#define LIBFENSTER_ICONS

#include <cairo/cairo.h>
#include "color_argb.hpp"

namespace fenster
{
// Resolution-independent icons share a 24 px grid and a consistent stroke weight.
enum class Icon : uint32_t
{
	None, Back, Forward, Up, Down, Refresh, Grid, List, Search, Close,
	Home, Desktop, Applications, Computer, Folder, Document, Terminal, More,
	Check, Info, ChevronRight, NewWindow, Sort, Save, Undo, Redo
};

inline void paintIcon(cairo_t* cr, Icon icon, double x, double y, double size, ColorArgb color)
{
	if(icon == Icon::None || size <= 0) return;
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_scale(cr, size / 24., size / 24.);
	cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(color));
	cairo_set_line_width(cr, 1.7);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
	auto line = [&](double x1, double y1, double x2, double y2) {
		cairo_move_to(cr, x1, y1); cairo_line_to(cr, x2, y2); cairo_stroke(cr);
	};
	switch(icon)
	{
		case Icon::Back: case Icon::Forward: case Icon::Up: case Icon::Down:
			cairo_translate(cr, 12, 12);
			cairo_rotate(cr, icon == Icon::Forward ? 3.141592653589793 : icon == Icon::Up ? 1.570796326794897 : icon == Icon::Down ? -1.570796326794897 : 0);
			line(-7, 0, 7, 0); cairo_move_to(cr, -1, -6); cairo_line_to(cr, -7, 0); cairo_line_to(cr, -1, 6); cairo_stroke(cr);
			break;
		case Icon::Refresh:
			cairo_arc(cr, 12, 12, 7, .4, 5.5); cairo_stroke(cr);
			cairo_move_to(cr, 18, 4); cairo_line_to(cr, 18, 9); cairo_line_to(cr, 13, 9); cairo_stroke(cr); break;
		case Icon::Grid: case Icon::Applications:
			for(int row = 0; row < 2; ++row) for(int col = 0; col < 2; ++col) {
				cairo_rectangle(cr, 4 + col * 10, 4 + row * 10, 6, 6); cairo_stroke(cr);
			} break;
		case Icon::List:
			for(int row = 0; row < 3; ++row) {
				cairo_arc(cr, 4, 6 + row * 6, .8, 0, 6.283185307); cairo_fill(cr);
				line(9, 6 + row * 6, 20, 6 + row * 6);
			} break;
		case Icon::Search:
			cairo_arc(cr, 10.5, 10.5, 6, 0, 6.283185307); cairo_stroke(cr); line(15, 15, 20, 20); break;
		case Icon::Close: line(6, 6, 18, 18); line(18, 6, 6, 18); break;
		case Icon::Home:
			cairo_move_to(cr, 3, 11); cairo_line_to(cr, 12, 4); cairo_line_to(cr, 21, 11); cairo_stroke(cr);
			cairo_move_to(cr, 6, 10); cairo_line_to(cr, 6, 20); cairo_line_to(cr, 10, 20);
			cairo_line_to(cr, 10, 14); cairo_line_to(cr, 14, 14); cairo_line_to(cr, 14, 20);
			cairo_line_to(cr, 18, 20); cairo_line_to(cr, 18, 10); cairo_stroke(cr); break;
		case Icon::Desktop: case Icon::Computer:
			cairo_rectangle(cr, 3, 4, 18, 13); cairo_stroke(cr); line(12, 17, 12, 21); line(8, 21, 16, 21); break;
		case Icon::Folder:
			cairo_set_source_rgb(cr, .91, .65, .24);
			cairo_move_to(cr, 2, 6); cairo_curve_to(cr, 2, 4.5, 2.5, 4, 4, 4);
			cairo_line_to(cr, 9, 4); cairo_line_to(cr, 11, 7); cairo_line_to(cr, 20, 7);
			cairo_curve_to(cr, 21.5, 7, 22, 7.5, 22, 9); cairo_line_to(cr, 22, 18);
			cairo_curve_to(cr, 22, 19.5, 21.5, 20, 20, 20); cairo_line_to(cr, 4, 20);
			cairo_curve_to(cr, 2.5, 20, 2, 19.5, 2, 18); cairo_close_path(cr); cairo_fill(cr);
			cairo_set_source_rgb(cr, 1, .78, .36);
			cairo_move_to(cr, 2, 9); cairo_line_to(cr, 22, 9); cairo_line_to(cr, 22, 18);
			cairo_curve_to(cr, 22, 19.5, 21.5, 20, 20, 20); cairo_line_to(cr, 4, 20);
			cairo_curve_to(cr, 2.5, 20, 2, 19.5, 2, 18); cairo_close_path(cr); cairo_fill(cr); break;
		case Icon::Document:
			cairo_set_source_rgb(cr, .96, .97, 1); cairo_move_to(cr, 5, 2); cairo_line_to(cr, 14, 2);
			cairo_line_to(cr, 20, 8); cairo_line_to(cr, 20, 22); cairo_line_to(cr, 5, 22); cairo_close_path(cr); cairo_fill_preserve(cr);
			cairo_set_source_rgb(cr, .58, .65, .77); cairo_set_line_width(cr, 1); cairo_stroke(cr);
			cairo_move_to(cr, 14, 2); cairo_line_to(cr, 14, 8); cairo_line_to(cr, 20, 8); cairo_stroke(cr);
			line(8, 12, 16, 12); line(8, 16, 16, 16); break;
		case Icon::Terminal:
			cairo_rectangle(cr, 3, 4, 18, 16); cairo_stroke(cr);
			cairo_move_to(cr, 7, 9); cairo_line_to(cr, 10, 12); cairo_line_to(cr, 7, 15); cairo_stroke(cr); line(13, 15, 17, 15); break;
		case Icon::More:
			for(int i = 0; i < 3; ++i) { cairo_arc(cr, 5 + i * 7, 12, 1, 0, 6.283185307); cairo_fill(cr); } break;
		case Icon::Check:
			cairo_move_to(cr, 5, 12); cairo_line_to(cr, 10, 17); cairo_line_to(cr, 20, 6); cairo_stroke(cr); break;
		case Icon::Info:
			cairo_arc(cr, 12, 12, 9, 0, 6.283185307); cairo_stroke(cr); line(12, 11, 12, 17);
			cairo_arc(cr, 12, 7, .8, 0, 6.283185307); cairo_fill(cr); break;
		case Icon::ChevronRight:
			cairo_move_to(cr, 9, 6); cairo_line_to(cr, 15, 12); cairo_line_to(cr, 9, 18); cairo_stroke(cr); break;
		case Icon::NewWindow:
			cairo_rectangle(cr, 3, 7, 14, 14); cairo_stroke(cr); line(7, 3, 21, 3); line(21, 3, 21, 17); break;
		case Icon::Sort:
			line(4, 6, 20, 6); line(4, 12, 15, 12); line(4, 18, 10, 18); break;
		case Icon::Save:
			cairo_move_to(cr, 4, 3); cairo_line_to(cr, 17, 3); cairo_line_to(cr, 21, 7);
			cairo_line_to(cr, 21, 21); cairo_line_to(cr, 4, 21); cairo_close_path(cr); cairo_stroke(cr);
			cairo_rectangle(cr, 8, 3, 8, 6); cairo_stroke(cr); cairo_rectangle(cr, 8, 14, 9, 7); cairo_stroke(cr); break;
		case Icon::Undo: case Icon::Redo:
			if(icon == Icon::Redo) { cairo_translate(cr, 24, 0); cairo_scale(cr, -1, 1); }
			cairo_move_to(cr, 4, 10); cairo_line_to(cr, 14, 10); cairo_curve_to(cr, 23, 10, 23, 20, 13, 20); cairo_stroke(cr);
			cairo_move_to(cr, 9, 5); cairo_line_to(cr, 4, 10); cairo_line_to(cr, 9, 15); cairo_stroke(cr); break;
		default: break;
	}
	cairo_restore(cr);
}
}
#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/desktop/selection.hpp"
#include <cairo/cairo.h>

namespace fensterserver
{
	void Selection::paint()
	{
		cairo_t* cr = graphics.acquireContext();
		if(!cr)
			return;

		auto bounds = getBounds();

		cairo_save(cr);
		cairo_set_source_rgba(cr, 0, 0, 0, 0);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);

		static const double dash[] = {2.0};
		cairo_set_source_rgba(cr, 1, 1, 1, 1);
		cairo_set_line_width(cr, 1.0);
		cairo_set_dash(cr, dash, 1, 0);
		cairo_rectangle(cr, 1.5, 1.5, bounds.width - 2, bounds.height - 2);
		cairo_stroke(cr);

		graphics.releaseContext();
	}
}

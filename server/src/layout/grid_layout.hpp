// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_GRIDLAYOUT
#define FENSTER_SERVER_LAYOUT_GRIDLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/spacing_support.hpp"

namespace fensterserver {
	class GridLayout : public Layout, public PaddingSupport, public SpacingSupport {
		int columns = 1;
		int rows = 0;

	public:
		bool debug = false;

		explicit GridLayout(int columns = 1, int rows = 0) : columns(columns), rows(rows),
		                                             PaddingSupport(fenster::Insets(0, 0, 0, 0)),
		                                             SpacingSupport(0, 0) {
		}

		explicit GridLayout(fenster::Insets insets) : PaddingSupport(insets),
		                                              SpacingSupport(0, 0) {
		}

		explicit GridLayout(int columns, int rows, int horizontalSpace, int verticalSpace)
			: columns(columns), rows(rows), PaddingSupport(fenster::Insets(0, 0, 0, 0)),
			  SpacingSupport(horizontalSpace, verticalSpace) {
		}

		void layout() override;
	};
}

#endif

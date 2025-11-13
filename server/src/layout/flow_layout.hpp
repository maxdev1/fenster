// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_FLOWLAYOUT
#define FENSTER_SERVER_LAYOUT_FLOWLAYOUT

#include "layout.hpp"
#include "support/padding_support.hpp"
#include "support/spacing_support.hpp"

namespace fensterserver {
	class FlowLayout : public Layout, public PaddingSupport, public SpacingSupport {
	public:
		explicit FlowLayout(int horizontalSpace = 0, int verticalSpace = 0) : SpacingSupport(
			horizontalSpace, verticalSpace) {
		}

		void layout() override;
	};
}

#endif

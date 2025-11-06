// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_PROPERTIES
#define LIBFENSTER_PROPERTIES

#include <cstdint>

namespace fenster
{
	enum class ComponentProperty : uint32_t
	{
		/**
		 * Properties may have a different meaning for each component. They are
		 * used to simplify configuring components from a client application.
		*/
		Resizable,
		Secure,
		Enabled,
		Layout,
		Background,
		Color,
		Alignment,
		Focusable,
		Focused,
		Visible,
		DispatchesFocus,
		ImageSource,
		Style,
		Title,
		Checked,

		/**
		 * Properties for layout managers that also might have a different meaning
		 * per layout manager.
		*/
		LayoutOrientation,
		LayoutSpace,
		LayoutSpaceHorizontal,
		LayoutSpaceVertical,
		LayoutPadding,
		LayoutFlexComponentInfo
	};

}

#endif

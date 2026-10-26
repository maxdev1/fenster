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
		Variant,
		Title,
		Checked,
		MultiLine,

		/**
		 * Properties for layout managers that also might have a different meaning
		 * per layout manager.
		 */
		LayoutOrientation,
		LayoutSpace,
		LayoutSpaceHorizontal,
		LayoutSpaceVertical,
		LayoutPadding,
		LayoutFlexComponentInfo,

		Icon,
		Selected,
		CornerRadius,
		BorderColor,
		HoverBackground,
		SelectedBackground,
		FontSize,
		Placeholder,
		Tooltip,
		SelectAll,
		ScrollOffsetY,
		MouseCursor,
		ShowInTaskbar,
		/**
		 * Promotes a child of the registered desktop to a dock above ordinary windows.
		 */
		DesktopTaskbar,
		/**
		 * Shows a child of the desktop dock as a focusable popup, or dismiss it.
		 */
		DesktopPopup,
		MinimizeButtonVisible,
		MaximizeButtonVisible
	};

}

#endif

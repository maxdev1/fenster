// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/selection.hpp"

namespace fenster
{
	Selection* Selection::create()
	{
		return createComponent<Selection, FENSTER_COMPONENT_TYPE_SELECTION>();
	}
}

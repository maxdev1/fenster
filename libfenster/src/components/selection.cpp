// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/components/selection.hpp"

namespace fenster
{
	Selection* Selection::create()
	{
		return createComponent<Selection, ComponentType::Selection>();
	}
}

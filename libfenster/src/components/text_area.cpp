// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/components/text_area.hpp"

namespace fenster
{
	TextArea* TextArea::create()
	{
		return createComponent<TextArea, ComponentType::TextArea>();
	}

}

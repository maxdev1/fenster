// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_DESKTOP_SELECTION
#define FENSTER_SERVER_COMPONENTS_DESKTOP_SELECTION

#include "components/component.hpp"

namespace fensterserver
{
	class Selection : virtual public Component
	{
	public:
		virtual void paint();
	};
}

#endif

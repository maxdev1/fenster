// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_DESKTOP_SELECTION__
#define __WINDOWSERVER_COMPONENTS_DESKTOP_SELECTION__

#include "components/component.hpp"

class selection_t : virtual public component_t
{
  public:
	virtual void paint();
};

#endif

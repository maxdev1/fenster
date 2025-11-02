// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_EVENTS_KEYEVENT__
#define __WINDOWSERVER_EVENTS_KEYEVENT__

#include "events/event.hpp"

#include "platform/platform.hpp"
#include <stdint.h>

class key_event_t : public event_t
{
  public:
	g_key_info info;

	virtual ~key_event_t() {}

	virtual component_t *visit(component_t *component);
};

#endif

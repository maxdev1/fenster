// fenster — lightweight window server and UI toolkit
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENT_UPDATES
#define FENSTER_SERVER_COMPONENT_UPDATES

#include <libfenster/interface.hpp>
#include <functional>

namespace fensterserver
{
	bool applyComponentUpdates(const uint8_t* bytes, size_t length,
	                           const std::function<bool(const uint8_t*, size_t)>& apply);
}

#endif

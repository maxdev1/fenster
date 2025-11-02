// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "application_exit_cleanup.hpp"
#include "component_registry.hpp"
#include "process_registry.hpp"

#include <libfenster/interface.hpp>

namespace fensterserver
{
	void interfaceApplicationExitCleanupThread(ApplicationExitCleanupHandler* handler)
	{
		handler->run();
	}

	void ApplicationExitCleanupHandler::run()
	{
		fenster::platformJoin(pid);
		ComponentRegistry::cleanupProcess(pid);
		ProcessRegistry::cleanup_process(pid);
	}
}

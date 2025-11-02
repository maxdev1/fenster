// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "application_exit_cleanup.hpp"
#include "component_registry.hpp"
#include "process_registry.hpp"

#include <libwindow/interface.hpp>

void interfaceApplicationExitCleanupThread(application_exit_cleanup_handler_t* handler)
{
	handler->run();
}

void application_exit_cleanup_handler_t::run()
{
	platformJoin(pid);
	component_registry_t::cleanupProcess(pid);
	process_registry_t::cleanup_process(pid);
}

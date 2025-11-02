// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "process_registry.hpp"

#include <map>

static std::map<SYS_TID_T, SYS_TID_T> remoteDelegates;
static SYS_MUTEX_T remoteDelegatesLock = platformInitializeMutex(false);

void process_registry_t::bind(SYS_TID_T pid, SYS_TID_T tid)
{
	platformAcquireMutex(remoteDelegatesLock);
	remoteDelegates[pid] = tid;
	platformReleaseMutex(remoteDelegatesLock);
}

SYS_TID_T process_registry_t::get(SYS_TID_T pid)
{
	platformAcquireMutex(remoteDelegatesLock);
	if(remoteDelegates.count(pid) > 0)
	{
		SYS_TID_T tid = remoteDelegates[pid];
		platformReleaseMutex(remoteDelegatesLock);
		return tid;
	}
	platformReleaseMutex(remoteDelegatesLock);
	return SYS_TID_NONE;
}

void process_registry_t::cleanup_process(SYS_TID_T pid)
{
	platformAcquireMutex(remoteDelegatesLock);
	remoteDelegates.erase(pid);
	platformReleaseMutex(remoteDelegatesLock);
}

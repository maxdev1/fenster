// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "process_registry.hpp"

#include <map>

namespace fensterserver
{
	static std::map<SYS_TID_T, SYS_TID_T> remoteDelegates;
	static SYS_MUTEX_T remoteDelegatesLock = fenster::platformInitializeMutex(false);

	void ProcessRegistry::bind(SYS_TID_T pid, SYS_TID_T tid)
	{
		fenster::platformAcquireMutex(remoteDelegatesLock);
		remoteDelegates[pid] = tid;
		fenster::platformReleaseMutex(remoteDelegatesLock);
	}

	SYS_TID_T ProcessRegistry::get(SYS_TID_T pid)
	{
		fenster::platformAcquireMutex(remoteDelegatesLock);
		if(remoteDelegates.count(pid) > 0)
		{
			SYS_TID_T tid = remoteDelegates[pid];
			fenster::platformReleaseMutex(remoteDelegatesLock);
			return tid;
		}
		fenster::platformReleaseMutex(remoteDelegatesLock);
		return SYS_TID_NONE;
	}

	void ProcessRegistry::cleanup_process(SYS_TID_T pid)
	{
		fenster::platformAcquireMutex(remoteDelegatesLock);
		remoteDelegates.erase(pid);
		fenster::platformReleaseMutex(remoteDelegatesLock);
	}
}
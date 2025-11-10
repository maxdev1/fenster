// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _WIN32

#include "libfenster/platform/platform.hpp"
#include "libfenster/interface.hpp"
#include <cstdarg>
#include <stdio.h>

namespace fenster
{
	SYS_TID_T platformWaitForServer()
	{
		return NULL;
	}

	std::string platformGetFontPath(std::string fontName)
	{
		return "platform/sim/fonts/" + fontName + ".ttf";
	}

	void platformLog(const char* message, ...)
	{
		va_list l;
		va_start(l, message);
		vprintf(message, l);
		va_end(l);
		printf("\n");
	}

	SYS_MUTEX_T platformInitializeMutex(bool reentrant)
	{
		win_mutex_t* mutex = (win_mutex_t*) malloc(sizeof(win_mutex_t));
		mutex->reentrant = reentrant;
		if(reentrant)
			InitializeCriticalSection(&mutex->cs);
		else
			mutex->handle = CreateSemaphore(NULL, 1, 1, NULL);
		return mutex;
	}

	void platformAcquireMutex(SYS_MUTEX_T m)
	{
		if(m->reentrant)
		{
			EnterCriticalSection(&m->cs);
		}
		else
		{
			WaitForSingleObject(m->handle, INFINITE);
		}
	}

	void platformAcquireMutexTimeout(SYS_MUTEX_T m, uint32_t timeoutMs)
	{
		if (m->reentrant) {
			uint32_t waited = 0;
			while (true) {
				if (TryEnterCriticalSection(&m->cs))
					return;

				Sleep(1);

				if (++waited >= timeoutMs)
					return;
			}
		} else {
			WaitForSingleObject(m->handle, timeoutMs);
		}
	}

	void platformReleaseMutex(SYS_MUTEX_T m)
	{
		if(m->reentrant)
		{
			LeaveCriticalSection(&m->cs);
		}
		else
		{
			ReleaseSemaphore(m->handle, 1, NULL);
		}
	}


	DWORD WINAPI _platformThreadEntry(LPVOID arg)
	{
		void (*entry)() = (void (*)()) arg;
		entry();
		return 0;
	}

	SYS_TID_T platformCreateThread(void* entry)
	{
		return CreateThread(NULL, 0, _platformThreadEntry, entry, 0, NULL);
	}

	SYS_TID_T platformCreateThreadWithData(void* entry, void* data)
	{
		printf("NOT IMPLEMENTED: platformCreateThreadWithData\n");
	}

	uint64_t platformMillis()
	{
		return GetTickCount();
	}

	void platformSleep(uint64_t time)
	{
		Sleep(time);
	}

	// The following are mostly relevant for interfacing, so for now not implemented in the simulators

	SYS_TX_T platformCreateMessageTransaction()
	{
		printf("NOT IMPLEMENTED: platformCreateMessageTransaction\n");
	}

	bool platformSendMessage(SYS_TID_T tid, void* buf, size_t len, SYS_TX_T tx)
	{
		printf("NOT IMPLEMENTED: platformSendMessage\n");
	}

	int platformReceiveMessage(void* buf, size_t max, SYS_TX_T tx)
	{
		printf("NOT IMPLEMENTED: platformReceiveMessage\n");

		for(;;)
			platformSleep(1000);
	}

	void platformYieldTo(SYS_TID_T tid)
	{
		printf("NOT IMPLEMENTED: platformYieldTo\n");
	}

	void platformUnmapSharedMemory(void* mem)
	{
		printf("NOT IMPLEMENTED: platformUnmapSharedMemory\n");
	}

	SYS_TID_T platformGetPidForTid(SYS_TID_T tid)
	{
		printf("NOT IMPLEMENTED: platformGetPidForTid\n");
	}

	void* platformAllocateMemory(size_t size)
	{
		printf("NOT IMPLEMENTED: platformAllocateMemory\n");
		return nullptr;
	}

	void* platformShareMemory(void* memory, size_t size, SYS_TID_T target)
	{
		printf("NOT IMPLEMENTED: platformShareMemory\n");
		return nullptr;
	}

	void platformJoin(SYS_TID_T tid)
	{
		printf("NOT IMPLEMENTED: platformJoin\n");
	}

	bool platformRegisterTaskIdentifier(const char* task)
	{
		printf("NOT IMPLEMENTED: platformRegisterTaskIdentifier\n");
		return true;
	}

	SYS_TID_T platformSpawn(const char* path, const char* args, const char* cwd)
	{
		printf("NOT IMPLEMENTED: platformSpawn\n");
		return nullptr;
	}

	SYS_TID_T platformGetTid()
	{
		printf("NOT IMPLEMENTED: platformGetTid\n");
		return nullptr;
	}

	void platformExit(int v)
	{
		printf("NOT IMPLEMENTED: platformExit\n");
	}
}


#endif

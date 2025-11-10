// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef __APPLE__

#include "libfenster/platform/platform.hpp"
#include "libfenster/interface.hpp"
#include <cstdarg>
#include <stdio.h>
#include <time.h>

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
        mac_mutex_t* mutex = (mac_mutex_t*)malloc(sizeof(mac_mutex_t));

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);

        if (reentrant)
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        else
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);

        pthread_mutex_init(&mutex->m, &attr);

        pthread_mutexattr_destroy(&attr);
        return mutex;
    }

    void platformAcquireMutex(SYS_MUTEX_T m)
    {
        pthread_mutex_lock(&m->m);
    }

    void platformAcquireMutexTimeout(SYS_MUTEX_T m, uint32_t timeoutMs)
    {
		// TODO
    }

    void platformReleaseMutex(SYS_MUTEX_T m)
    {
        pthread_mutex_unlock(&m->m);
    }

    static void* _platformThreadEntry(void* arg)
    {
        void (*entry)() = (void (*)())arg;
        entry();
        return NULL;
    }

    SYS_TID_T platformCreateThread(void* entry)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, _platformThreadEntry, entry);
        return tid;
    }

	SYS_TID_T platformCreateThreadWithData(void* entry, void* data)
	{
		printf("NOT IMPLEMENTED: platformCreateThreadWithData\n");
	}

    uint64_t platformMillis()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
    }

    void platformSleep(uint64_t time)
    {
        struct timespec ts;
        ts.tv_sec = time / 1000ULL;
        ts.tv_nsec = (time % 1000ULL) * 1000000ULL;
        nanosleep(&ts, NULL);
    }

	void platformExit(int v)
	{
		exit(v);
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
}


#endif

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
        auto mutex = new mac_mutex_t;
        mutex->reentrant = reentrant;
        mutex->available = true;
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        if(reentrant)
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mutex->m, &attr);
        pthread_mutexattr_destroy(&attr);
        pthread_cond_init(&mutex->ready, nullptr);
        return mutex;
    }

    void platformAcquireMutex(SYS_MUTEX_T mutex)
    {
        pthread_mutex_lock(&mutex->m);
        if(!mutex->reentrant)
        {
            while(!mutex->available)
                pthread_cond_wait(&mutex->ready, &mutex->m);
            mutex->available = false;
            pthread_mutex_unlock(&mutex->m);
        }
    }

    void platformAcquireMutexTimeout(SYS_MUTEX_T mutex, uint32_t timeoutMs)
    {
        if(!timeoutMs)
            return platformAcquireMutex(mutex);
        if(mutex->reentrant)
        {
            const auto deadline = platformMillis() + timeoutMs;
            while(pthread_mutex_trylock(&mutex->m) != 0)
            {
                if(platformMillis() >= deadline)
                    return;
                platformSleep(1);
            }
            return;
        }
        timespec deadline;
        clock_gettime(CLOCK_REALTIME, &deadline);
        deadline.tv_sec += timeoutMs / 1000;
        deadline.tv_nsec += (timeoutMs % 1000) * 1000000L;
        deadline.tv_sec += deadline.tv_nsec / 1000000000L;
        deadline.tv_nsec %= 1000000000L;
        pthread_mutex_lock(&mutex->m);
        while(!mutex->available)
            if(pthread_cond_timedwait(&mutex->ready, &mutex->m, &deadline) != 0)
                break;
        if(mutex->available)
            mutex->available = false;
        pthread_mutex_unlock(&mutex->m);
    }

    void platformReleaseMutex(SYS_MUTEX_T mutex)
    {
        if(!mutex->reentrant)
        {
            pthread_mutex_lock(&mutex->m);
            mutex->available = true;
            pthread_cond_signal(&mutex->ready);
        }
        pthread_mutex_unlock(&mutex->m);
    }

    void platformDestroyMutex(SYS_MUTEX_T mutex)
    {
        pthread_cond_destroy(&mutex->ready);
        pthread_mutex_destroy(&mutex->m);
        delete mutex;
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

	struct DataThreadEntry { void (*entry)(void*); void* data; };
	static void* dataThreadEntry(void* raw)
	{
		auto argument = static_cast<DataThreadEntry*>(raw);
		auto entry = *argument;
		delete argument;
		entry.entry(entry.data);
		return nullptr;
	}
	SYS_TID_T platformCreateThreadWithData(void* entry, void* data)
	{
		auto argument = new DataThreadEntry{reinterpret_cast<void(*)(void*)>(entry), data};
		pthread_t tid;
		if(pthread_create(&tid, nullptr, dataThreadEntry, argument) != 0)
		{
			delete argument;
			return SYS_TID_NONE;
		}
		return tid;
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
		pthread_join(tid, nullptr);
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

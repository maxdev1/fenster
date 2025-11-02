// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _GHOST_

#include "libfenster/platform/platform.hpp"
#include "libfenster/interface.hpp"

#include <cstdarg>
#include <ghost.h>

namespace fenster
{
	SYS_TID_T platformWaitForServer() {
		return g_task_await_by_name(G_UI_REGISTRY_NAME);
	}

	std::string platformGetFontPath(std::string fontName) {
		return "/system/graphics/fonts/" + fontName + ".ttf";
	}

	void platformLog(const char* message, ...) {
		va_list l;
		va_start(l, message);
		kvlog(message, l);
		va_end(l);
	}

	SYS_TID_T platformGetPidForTid(SYS_TID_T tid)
	{
		return g_get_pid_for_tid(tid);
	}

	void* platformAllocateMemory(size_t size)
	{
		return g_alloc_mem(size);
	}

	void* platformShareMemory(void* memory, size_t size, SYS_TID_T target)
	{
		return g_share_mem(memory, size, target);
	}

	SYS_TID_T platformCreateThread(void* entry)
	{
		return g_create_task(entry);
	}

	SYS_TID_T platformCreateThreadWithData(void* entry, void* data)
	{
		return g_create_task_d(entry, data);
	}

	SYS_TX_T platformCreateMessageTransaction()
	{
		return g_get_message_tx_id();
	}

	bool platformSendMessage(SYS_TID_T tid, void* buf, size_t len, SYS_TX_T tx)
	{
		return g_send_message_t(tid, buf, len, tx) == G_MESSAGE_SEND_STATUS_SUCCESSFUL;
	}

	int platformReceiveMessage(void* buf, size_t max, SYS_TX_T tx)
	{
		return g_receive_message_t(buf, max, tx);
	}

	SYS_MUTEX_T platformInitializeMutex(bool reentrant)
	{
		return g_mutex_initialize_r(reentrant);
	}

	void platformAcquireMutex(SYS_MUTEX_T mutex)
	{
		g_mutex_acquire(mutex);
	}

	void platformAcquireMutexTimeout(SYS_MUTEX_T mutex, uint32_t timeout)
	{
		g_mutex_acquire_to(mutex, timeout);
	}

	void platformReleaseMutex(SYS_MUTEX_T mutex)
	{
		g_mutex_release(mutex);
	}

	uint64_t platformMillis()
	{
		return g_millis();
	}

	void platformJoin(SYS_TID_T tid)
	{
		g_join(tid);
	}

	bool platformRegisterTaskIdentifier(const char* identifier)
	{
		return g_task_register_name(identifier);
	}

	SYS_TID_T platformSpawn(const char* path, const char* args, const char* cwd)
	{
		return g_spawn(path, args, cwd, G_SECURITY_LEVEL_APPLICATION);
	}

	void platformYieldTo(SYS_TID_T tid)
	{
		g_yield_t(tid);
	}

	void platformUnmapSharedMemory(void* mem)
	{
		g_unmap(mem);
	}

	void platformSleep(uint64_t time)
	{
		g_sleep(time);
	}

	SYS_TID_T platformGetTid()
	{
		return g_get_tid();
	}

	void platformExit(int v)
	{
		g_exit(v);
	}
}

#endif

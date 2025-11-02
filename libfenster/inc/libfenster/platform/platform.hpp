// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_PLATFORM_PLATFORM
#define LIBFENSTER_PLATFORM_PLATFORM

#include <cstdint>

/**
 * Ghost-specific type definitions
 */
#ifdef _GHOST_

#include <ghost.h>
#include <libinput/keyboard/keyboard.hpp>
#include <libinput/mouse/mouse.hpp>

#define SYS_TID_T                                 g_tid
#define SYS_TID_NONE                              G_TID_NONE

#define SYS_MUTEX_T                               g_user_mutex

#define SYS_PAGE_SIZE                             G_PAGE_SIZE
#define SYS_PAGE_ALIGN_UP(address)                G_PAGE_ALIGN_UP(address)

#define SYS_TX_T                                  g_message_transaction
#define SYS_TX_NONE                               G_MESSAGE_TRANSACTION_NONE

#define SYS_MESSAGE_HEADER_SIZE                   sizeof(g_message_header)
#define SYS_MESSAGE_RECEIVE_SUCCESS               G_MESSAGE_RECEIVE_STATUS_SUCCESSFUL
#define SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER  G_MESSAGE_RECEIVE_STATUS_EXCEEDS_BUFFER_SIZE

#define SYS_MESSAGE_CONTENT(message)              G_MESSAGE_CONTENT(message)
#define SYS_MESSAGE_SENDER(message)               ((g_message_header*) message)->sender
#define SYS_MESSAGE_TRANSACTION(message)          ((g_message_header*) message)->transaction


/**
 * Windows-MinGW-specific type definitions
 */
#elif _WIN32

#include <windows.h>
#include <string>

#define SYS_TID_T                                 HANDLE
#define SYS_TID_NONE                              NULL

typedef struct {
 HANDLE handle;
 CRITICAL_SECTION cs;
 bool reentrant;
} win_mutex_t;
#define SYS_MUTEX_T win_mutex_t*

#define SYS_PAGE_SIZE                             4096
#define SYS_PAGE_ALIGN_DOWN(value)	              ((value) & ~(SYS_PAGE_SIZE - 1))
#define SYS_PAGE_ALIGN_UP(value)	              (((value) & (SYS_PAGE_SIZE - 1)) ? (SYS_PAGE_ALIGN_DOWN((value)) + SYS_PAGE_SIZE) : (value))

#define SYS_TX_T                                  int // TODO
#define SYS_TX_NONE                               -1 // TODO

#define SYS_MESSAGE_HEADER_SIZE                   0 // TODO
#define SYS_MESSAGE_RECEIVE_SUCCESS               1 // TODO
#define SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER  2 // TODO

#define SYS_MESSAGE_CONTENT(message)              message // TODO
#define SYS_MESSAGE_SENDER(message)               0 // TODO
#define SYS_MESSAGE_TRANSACTION(message)          0 // TODO

#define M_PI 3.14159265358979323846

#endif

namespace fenster
{
	/**
	 * Called by clients to wait until the server is available. Freezes the caller until the
	 * server task is ready to process requests.
	 */
	SYS_TID_T platformWaitForServer();

	/**
	 * Logs a message to the platforms log.
	 */
	void platformLog(const char* message, ...);

	/**
	 *
	 */
	SYS_MUTEX_T platformInitializeMutex(bool reentrant);

	void platformAcquireMutex(SYS_MUTEX_T mutex);

	void platformAcquireMutexTimeout(SYS_MUTEX_T mutex, uint32_t timeout);

	void platformReleaseMutex(SYS_MUTEX_T mutex);

	SYS_TX_T platformCreateMessageTransaction();

	bool platformSendMessage(SYS_TID_T tid, void* buf, size_t len, SYS_TX_T tx);

	int platformReceiveMessage(void* buf, size_t max, SYS_TX_T tx);

	void platformYieldTo(SYS_TID_T tid);

	void platformUnmapSharedMemory(void* mem);

	SYS_TID_T platformCreateThread(void* entry);

	SYS_TID_T platformCreateThreadWithData(void* entry, void* data);

	std::string platformGetFontPath(std::string fontName);

	SYS_TID_T platformGetPidForTid(SYS_TID_T tid);

	void* platformAllocateMemory(size_t size);

	void* platformShareMemory(void* memory, size_t size, SYS_TID_T target);

	uint64_t platformMillis();

	void platformJoin(SYS_TID_T tid);

	bool platformRegisterTaskIdentifier(const char* task);

	SYS_TID_T platformSpawn(const char* path, const char* args, const char* cwd);

	void platformSleep(uint64_t time);

	SYS_TID_T platformGetTid();

	void platformExit(int v);
}

#endif

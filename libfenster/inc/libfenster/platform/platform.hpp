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

#define SYS_TID_T g_tid
#define SYS_TID_NONE G_TID_NONE

#define SYS_MUTEX_T g_user_mutex

#define SYS_PAGE_SIZE G_PAGE_SIZE
#define SYS_PAGE_ALIGN_UP(address) G_PAGE_ALIGN_UP(address)

#define SYS_TX_T g_message_transaction
#define SYS_TX_NONE G_MESSAGE_TRANSACTION_NONE

#define SYS_MESSAGE_HEADER_SIZE sizeof(g_message_header)
#define SYS_MESSAGE_RECEIVE_SUCCESS G_MESSAGE_RECEIVE_STATUS_SUCCESSFUL
#define SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER G_MESSAGE_RECEIVE_STATUS_EXCEEDS_BUFFER_SIZE

#define SYS_MESSAGE_CONTENT(message) G_MESSAGE_CONTENT(message)
#define SYS_MESSAGE_SENDER(message) ((g_message_header*) message)->sender
#define SYS_MESSAGE_TRANSACTION(message) ((g_message_header*) message)->transaction


/**
 * Windows-MinGW-specific type definitions
 */
#elif _WIN32

#include <string>
#include <windows.h>

#define SYS_TID_T HANDLE
#define SYS_TID_NONE NULL

typedef struct
{
	HANDLE handle;
	CRITICAL_SECTION cs;
	bool reentrant;
} win_mutex_t;
#define SYS_MUTEX_T win_mutex_t*

#define SYS_PAGE_SIZE 4096
#define SYS_PAGE_ALIGN_DOWN(value) ((value) & ~(SYS_PAGE_SIZE - 1))
#define SYS_PAGE_ALIGN_UP(value)                                                                                       \
	(((value) & (SYS_PAGE_SIZE - 1)) ? (SYS_PAGE_ALIGN_DOWN((value)) + SYS_PAGE_SIZE) : (value))

#define SYS_TX_T int // TODO
#define SYS_TX_NONE -1 // TODO

#define SYS_MESSAGE_HEADER_SIZE 0 // TODO
#define SYS_MESSAGE_RECEIVE_SUCCESS 1 // TODO
#define SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER 2 // TODO

#define SYS_MESSAGE_CONTENT(message) message // TODO
#define SYS_MESSAGE_SENDER(message) 0 // TODO
#define SYS_MESSAGE_TRANSACTION(message) 0 // TODO

#define M_PI 3.14159265358979323846

#endif

/**
 * Function stubs below must be implemented.
 */
namespace fenster
{
	/**
	 * Blocks the caller until the window server is available.
	 *
	 * This function suspends execution until the server task is ready to
	 * accept and process requests.
	 *
	 * @return The thread ID (SYS_TID_T) of the server task
	 */
	SYS_TID_T platformWaitForServer();

	/**
	 * Writes a formatted message to the platform's log output.
	 *
	 * @param message The message format string (printf-style)
	 * @param ... Additional arguments for the format string
	 */
	void platformLog(const char* message, ...);

	/**
	 * Initializes a mutex.
	 *
	 * @param reentrant If true, creates a reentrant mutex that can be acquired
	 *        multiple times by the same thread. Otherwise, the mutex must block
	 *        when the same thread attempts to acquire it twice.
	 * @return A handle to the created mutex object
	 */
	SYS_MUTEX_T platformInitializeMutex(bool reentrant);

	/**
	 * Acquires ownership of a mutex, blocking until it becomes available.
	 *
	 * @param mutex The mutex handle
	 */
	void platformAcquireMutex(SYS_MUTEX_T mutex);

	/**
	 * Attempts to acquire a mutex, blocking for a limited time.
	 *
	 * @param mutex The mutex handle
	 * @param timeout Timeout in milliseconds
	 */
	void platformAcquireMutexTimeout(SYS_MUTEX_T mutex, uint32_t timeout);

	/**
	 * Releases ownership of a mutex.
	 *
	 * @param mutex The mutex handle
	 */
	void platformReleaseMutex(SYS_MUTEX_T mutex);

	/**
	 * Generates a unique transaction ID for message exchange.
	 *
	 * Used to correlate requests and responses between processes.
	 *
	 * @return the transaction ID
	 */
	SYS_TX_T platformCreateMessageTransaction();

	/**
	 * Sends a message to a target thread.
	 *
	 * @param tid The target thread ID
	 * @param buffer Pointer to the message data
	 * @param length Length of the message data in bytes
	 * @param tx  Transaction identifier for this message or SYS_TX_NONE
	 *
	 * @return true on success, false otherwise
	 */
	bool platformSendMessage(SYS_TID_T tid, void* buffer, size_t length, SYS_TX_T tx);

	/**
	 * Receives a message from another process or thread.
	 *
	 * Blocks until a message is received, optionally filtering by transaction ID.
	 *
	 * @param buffer Buffer to receive message data
	 * @param max Maximum buffer size in bytes
	 * @param tx  Transaction ID to match, or SYS_TX_NONE to receive any message
	 *
	 * @return Number of bytes received
	 */
	int platformReceiveMessage(void* buffer, size_t max, SYS_TX_T tx);

	/**
	 * Yields the CPU, preferring the specified thread as the next to run.
	 *
	 * @param tid The thread to yield execution to
	 */
	void platformYieldTo(SYS_TID_T tid);

	/**
	 * Creates a new thread that starts execution at the given entry point.
	 *
	 * @param entry Pointer to the entry function
	 * @return The created thread's ID
	 */
	SYS_TID_T platformCreateThread(void* entry);

	/**
	 * Creates a new thread that starts execution at the given entry point, with user data.
	 *
	 * @param entry Pointer to the entry function
	 * @param data  Pointer to user data passed as the first argument
	 * @return The created thread's ID
	 */
	SYS_TID_T platformCreateThreadWithData(void* entry, void* data);

	/**
	 * Returns the process ID corresponding to a given thread ID.
	 */
	SYS_TID_T platformGetPidForTid(SYS_TID_T tid);

	/**
	 * Retrieves the ID of the calling thread.
	 */
	SYS_TID_T platformGetTid();

	/**
	 * Allocates shared memory.
	 *
	 * The returned memory can later be shared with other processes via
	 * platformShareMemory().
	 *
	 * @param size Number of bytes to allocate
	 * @return Pointer to the allocated memory
	 */
	void* platformAllocateMemory(size_t size);

	/**
	 * Unmaps a shared memory region from the current process.
	 *
	 * Other processes with shared access retain their mappings.
	 *
	 * @param mem Pointer to the memory region
	 */
	void platformUnmapSharedMemory(void* mem);

	/**
	 * Shares a memory region with another process.
	 *
	 * @param memory Pointer to the allocated memory
	 * @param size   Size of the memory region
	 * @param target Target thread or process ID
	 *
	 * @return A pointer to the shared memory in the target process
	 */
	void* platformShareMemory(void* memory, size_t size, SYS_TID_T target);

	/**
	 * @return the current system time in milliseconds.
	 */
	uint64_t platformMillis();

	/**
	 * Blocks until the specified thread terminates.
	 *
	 * @tid target thread ID
	 */
	void platformJoin(SYS_TID_T tid);

	/**
	 * Registers the current thread with a specific identifier.
	 *
	 * Used for example by Ghost OS for process discovery (e.g., to identify the window server).
	 *
	 * @param task Unique string identifier for the task
	 * @return true on success, false otherwise
	 */
	bool platformRegisterTaskIdentifier(const char* task);

	/**
	 * Spawns a new process.
	 *
	 * @param path Path to the executable
	 * @param args Command-line arguments
	 * @param cwd  Working directory for the new process
	 *
	 * @return Process ID of the spawned application
	 */
	SYS_TID_T platformSpawn(const char* path, const char* args, const char* cwd);

	/**
	 * Suspends the current thread for the given duration.
	 *
	 * @param time Duration in milliseconds
	 */
	void platformSleep(uint64_t time);

	/**
	 * Returns the filesystem path of a font by name.
	 *
	 * @param fontName The logical font name
	 * @return The absolute path to the font file
	 */
	std::string platformGetFontPath(std::string fontName);

	/**
	 * Terminates the current process.
	 *
	 * @param code Exit status code
	 */
	void platformExit(int code);
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_EVENTS_EVENTPROCESSOR
#define FENSTER_SERVER_EVENTS_EVENTPROCESSOR

#include <deque>
#include "platform/platform.hpp"
#include "libfenster/input/key_info.hpp"

#define DEFAULT_MULTICLICK_TIMESPAN 250

namespace fensterserver
{

    /**
     * The event queue is used to store any incoming events for
     * later processing.
     */
    class EventProcessor
    {
    public:
        uint32_t multiclickTimespan;

        EventProcessor();

        std::deque<fenster::KeyInfo> key_info_buffer;
        SYS_MUTEX_T key_info_buffer_lock = fenster::platformInitializeMutex(false);
        void bufferKeyEvent(fenster::KeyInfo keyInfo);

        void process();

        void translateKeyEvent(fenster::KeyInfo& info);
        void processKeyState();
        void processMouseState();
    };
}

#endif

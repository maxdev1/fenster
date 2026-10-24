// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_EVENTS_EVENTPROCESSOR
#define FENSTER_SERVER_EVENTS_EVENTPROCESSOR

#include <deque>
#include "platform/platform.hpp"
#include "libfenster/input/key_info.hpp"
#include <libfenster/interface.hpp>

#define DEFAULT_MULTICLICK_TIMESPAN 250

namespace fensterserver
{
    class Server;
    class Screen;
    class MouseEvent;

    /**
     * Event processor buffers and processes incoming events.
     */
    class EventProcessor
    {
        struct MouseSample
        {
            fenster::Point position;
            fenster::MouseButton buttons;
            int scroll;
            uint64_t arrivalMillis;
            bool buttonChange;
        };

        std::deque<MouseSample> mouseBuffer;
        SYS_MUTEX_T mouseBufferLock = fenster::platformInitializeMutex(false);
        fenster::MouseButton lastBufferedButtons = fenster::MouseButton::None;
        uint64_t lastClick = 0;
        int clickCount = 0;

        void handlePress(Server* instance, Screen* screen, MouseEvent& event, uint64_t arrivalMillis);
        void handleRelease(Server* instance, MouseEvent& event);
        void handleMoveOrDrag(Server* instance, Screen* screen, MouseEvent& event);

    public:
        uint32_t multiclickTimespan;

        EventProcessor();

        std::deque<fenster::KeyInfo> key_info_buffer;
        SYS_MUTEX_T key_info_buffer_lock = fenster::platformInitializeMutex(false);
        void bufferKeyEvent(fenster::KeyInfo keyInfo);
        void bufferMouseState(int scroll, uint64_t arrivalMillis);

        void process();

        void translateKeyEvent(fenster::KeyInfo& info);
        void processKeyState();
        void processMouseState();
    };
}

#endif

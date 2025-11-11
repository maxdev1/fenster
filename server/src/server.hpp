// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_SERVER
#define FENSTER_SERVER_SERVER

#include "components/component.hpp"
#include "components/desktop/screen.hpp"
#include "components/label.hpp"
#include "events/event_processor.hpp"
#include "video/video_output.hpp"

namespace fensterserver
{
    class item_container_t;

    /**
     *
     */
    class Server
    {
        SYS_TID_T updateTask = SYS_TID_NONE;
        SYS_TID_T renderTask = SYS_TID_NONE;

        SYS_MUTEX_T updateLock = fenster::platformInitializeMutex(false);
        SYS_MUTEX_T lazyUpdateLock = fenster::platformInitializeMutex(false);
        fenster::Rectangle lastInvalid;

        void initializeVideo();
        void createVitalComponents(fenster::Rectangle screenBounds);
        VideoOutput* findVideoOutput();
        static void startInputHandlers();

        void updateLoop(const fenster::Rectangle& screenBounds);
        void updateDebounceLoop() const;

        static void fpsCounter();

    public:
        VideoOutput* videoOutput;
        EventProcessor* eventProcessor;
        Screen* screen;
        Label* stateLabel;

        Server();

        /**
         * Sets up the windowing system by configuring a video output, setting up
         * the event processor and running the main loop. Each step of the main loop
         * includes an event handling and rendering sequence.
         */
        void launch();

        /**
         * When components have changed, requests the server to perform an update of
         * all components. This triggers the update to run immediately.
         */
        void requestUpdateImmediately() const;

        /**
         * Also requests an update, but debounces requests that come in very quickly.
         * This is used to avoid that for example a large amount of changes via the
         * interface cause too many updates.
         */
        void requestUpdateLater() const;

        static void setDebug(bool cond);
        static bool isDebug();

        /**
         * Blits the component state.
         */
        void output(Graphics* graphics);

        /**
         * Dispatches the given event to the component.
         *
         * @return the component that has handled the event or NULL
         */
        Component* dispatch(Component* component, Event& event);

        /**
         * Dispatches the given event upwards the component tree.
         */
        Component* dispatchUpwards(Component* component, Event& event);

        /**
         * Switches the focus from the currently focused component to a different
         * component. Must be used to ensure state is updated correctly.
         */
        Component* switchFocus(Component* pressed);

        /**
         * Returns the singleton instance of the window server.
         *
         * @return the instance
         */
        static Server* instance();

        static void startLazyUpdateLoop();
        static void startOtherTasks();
    };

    extern Server* server;
}

#endif

// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER__
#define __WINDOWSERVER__

#include "components/component.hpp"
#include "components/desktop/screen.hpp"
#include "components/label.hpp"
#include "events/event_processor.hpp"
#include "video/video_output.hpp"

class item_container_t;

/**
 *
 */
class windowserver_t
{
    SYS_TID_T updateTask = SYS_TID_NONE;
    SYS_TID_T renderTask = SYS_TID_NONE;

    SYS_MUTEX_T updateLock = platformInitializeMutex(false);
    SYS_MUTEX_T lazyUpdateLock = platformInitializeMutex(false);
    g_rectangle lastInvalid;

    void initializeVideo();
    void createVitalComponents(g_rectangle screenBounds);
    g_video_output* findVideoOutput();
    static void startInputHandlers();

    void updateLoop(const g_rectangle& screenBounds);
    void updateDebounceLoop() const;

    static void fpsCounter();

public:
    g_video_output* videoOutput;
    event_processor_t* eventProcessor;
    screen_t* screen;
    label_t* stateLabel;

    windowserver_t();

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
    void output(graphics_t* graphics);

    /**
     * Dispatches the given event to the component.
     *
     * @return the component that has handled the event or NULL
     */
    component_t* dispatch(component_t* component, event_t& event);

    /**
     * Dispatches the given event upwards the component tree.
     */
    component_t* dispatchUpwards(component_t* component, event_t& event);

    /**
     * Switches the focus from the currently focused component to a different
     * component. Must be used to ensure state is updated correctly.
     */
    component_t* switchFocus(component_t* to);

    /**
     * Returns the singleton instance of the window server.
     *
     * @return the instance
     */
    static windowserver_t* instance();

    static void startLazyUpdateLoop();
    static void startOtherTasks();
};

extern windowserver_t* server;

#endif

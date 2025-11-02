// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_CANVAS
#define FENSTER_SERVER_COMPONENTS_CANVAS

#include "components/component.hpp"

namespace fensterserver
{
    struct CanvasBufferInfo
    {
        uint8_t* localMapping = nullptr;
        uint8_t* remoteMapping = nullptr;
        uint16_t pages = 0;

        cairo_surface_t* surface = nullptr;
        uint16_t paintableWidth = 0;
        uint16_t paintableHeight = 0;
    };

    class Canvas;

    struct async_resizer_info_t
    {
        bool alive;
        SYS_MUTEX_T lock;
        SYS_MUTEX_T checkAtom;
        Canvas* canvas;
    };

    class Canvas : virtual public Component
    {
        SYS_TID_T partnerProcess;
        async_resizer_info_t* asyncInfo;

        SYS_MUTEX_T bufferLock = fenster::platformInitializeMutex(true);
        CanvasBufferInfo buffer{};
        bool bufferReady = false;

        void createNewBuffer(fenster::Rectangle& bounds, int width, int height);
        void notifyClientAboutNewBuffer();

        void checkBuffer();

    protected:
        bool hasGraphics() const override
        {
            return false;
        }

        bool isFocusableDefault() const override
        {
            return true;
        }

    public:
        explicit Canvas(SYS_TID_T partnerThread);
        ~Canvas() override;

        void handleBoundChanged(const fenster::Rectangle& oldBounds) override;

        void blit(Graphics* out, const fenster::Rectangle& absClip, const fenster::Point& positionOnParent) override;

        static void asyncBufferResizer(async_resizer_info_t* info);
        void requestBlit(fenster::Rectangle& area);
    };
}

#endif

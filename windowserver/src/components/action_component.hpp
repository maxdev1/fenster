// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_ACTIONCOMPONENT
#define FENSTER_SERVER_COMPONENTS_ACTIONCOMPONENT
#include "component.hpp"

namespace fensterserver
{
    typedef std::function<void()> InternalActionHandlerFunc;

    /**
     * An action component is capable of being observed by an action listener.
     * The component may fire actions which are dispatched to the registered
     * listener for processing.
     */
    class ActionComponent : virtual public Component
    {
        InternalActionHandlerFunc internalHandler;

    public:
        explicit ActionComponent() : internalHandler(nullptr)
        {
        }

        ~ActionComponent() override = default;

        virtual void fireAction();

        void setInternalActionHandler(InternalActionHandlerFunc handler)
        {
            this->internalHandler = std::move(handler);
        }
    };
}

#endif

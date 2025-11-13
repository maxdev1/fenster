// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_CHECKABLECOMPONENT
#define FENSTER_SERVER_COMPONENTS_CHECKABLECOMPONENT

#include "component.hpp"

namespace fensterserver
{
    typedef std::function<void(bool, bool)> InternalCheckedHandlerFunc;

    /**
     * A component that has a "checked" state, such as a checkbox or a radio button.
     */
    class CheckableComponent : virtual public Component
    {
        std::vector<InternalCheckedHandlerFunc> internalHandlers;

    public:
        explicit CheckableComponent() = default;

        ~CheckableComponent() override = default;

        virtual void setChecked(bool value, bool triggeredByGroup = false);
        virtual void setCheckedInternal(bool value) = 0;
        virtual bool isChecked() = 0;

        void addInternalCheckedHandler(InternalCheckedHandlerFunc handler)
        {
            this->internalHandlers.push_back(std::move(handler));
        }
    };
}

#endif

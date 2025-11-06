// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_FOCUSABLECOMPONENT
#define FENSTER_SERVER_COMPONENTS_FOCUSABLECOMPONENT

#include <cstdint>
#include <libfenster/properties.hpp>

namespace fensterserver
{
    class Component;

    /**
     *
     */
    class FocusableComponent
    {
        Component* self;

    protected:
        bool focused = false;
        bool focusable = true;
        bool dispatchesFocus = true;

    public:
        explicit FocusableComponent(Component* self) : self(self)
        {
        }

        virtual ~FocusableComponent() = default;

        virtual bool isFocused() const
        {
            return focused;
        }

        virtual void setFocusedInternal(bool focused)
        {
            this->focused = focused;
        }

        virtual bool isFocusable() const
        {
            return isFocusableDefault() && focusable;
        }

        virtual bool isFocusableDefault() const
        {
            return false;
        }

        void setDispatchesFocus(bool dispatches)
        {
            this->dispatchesFocus = dispatchesFocus;
        }

        bool isDispatchesFocus()
        {
            return this->dispatchesFocus;
        }

        Component* setFocused(bool focused);

        bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out);
        bool setNumericProperty(fenster::ComponentProperty property, uint32_t value);
    };
}

#endif

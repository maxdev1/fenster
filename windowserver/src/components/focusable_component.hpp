// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_FOCUSABLECOMPONENT__
#define __WINDOWSERVER_COMPONENTS_FOCUSABLECOMPONENT__
#include <stdint-gcc.h>

class component_t;

/**
 *
 */
class focusable_component_t
{
    component_t* self;

protected:
    bool focused = false;
    bool focusable = true;
    bool dispatchesFocus = true;

public:
    explicit focusable_component_t(component_t* self) : self(self)
    {
    }

    virtual ~focusable_component_t() = default;

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

    component_t* setFocused(bool focused);

    bool getNumericProperty(int property, uint32_t* out);
    bool setNumericProperty(int property, uint32_t value);
};

#endif

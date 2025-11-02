// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_TITLEDCOMPONENT__
#define __WINDOWSERVER_COMPONENTS_TITLEDCOMPONENT__

#include <string>

#include "component.hpp"

class titled_component_t : virtual public component_t
{
public:
    ~titled_component_t() override = default;

    virtual void setTitle(std::string title);
    virtual void setTitleInternal(std::string title) = 0;

    virtual std::string getTitle() = 0;
};

#endif

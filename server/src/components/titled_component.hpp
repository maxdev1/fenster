// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TITLEDCOMPONENT
#define FENSTER_SERVER_COMPONENTS_TITLEDCOMPONENT

#include <string>

#include "component.hpp"

namespace fensterserver
{
    class TitledComponent : virtual public Component
    {
    public:
        ~TitledComponent() override = default;

        virtual void setTitle(std::string title);
        virtual void setTitleInternal(std::string title) = 0;

        virtual std::string getTitle() = 0;
    };
}

#endif

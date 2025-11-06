// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_SINGLESPACINGSUPPORT
#define FENSTER_SERVER_LAYOUT_SINGLESPACINGSUPPORT

namespace fensterserver
{
    class SingleSpacingSupport
    {
    protected:
        int space = 0;

    public:
        SingleSpacingSupport() = default;

        explicit SingleSpacingSupport(int space): space(space)
        {
        }

        virtual ~SingleSpacingSupport() = default;

        virtual void setSpace(int space)
        {
            this->space = space;
        }

        virtual int getSpace()
        {
            return this->space;
        }
    };
}

#endif

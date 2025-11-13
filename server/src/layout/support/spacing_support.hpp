// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_LAYOUT_SPACINGSUPPORT
#define FENSTER_SERVER_LAYOUT_SPACINGSUPPORT

namespace fensterserver
{
    class SpacingSupport
    {
    protected:
        int horizontalSpace = 0;
        int verticalSpace = 0;

    public:
        explicit SpacingSupport(int horizontalSpace = 0, int verticalSpace = 0) : horizontalSpace(horizontalSpace),
            verticalSpace(verticalSpace)
        {
        }

        void setHorizontalSpace(int space)
        {
            this->horizontalSpace = space;
        }

        int getHorizontalSpace() const
        {
            return this->horizontalSpace;
        }

        void setVerticalSpace(int space)
        {
            this->verticalSpace = space;
        }

        int getVerticalSpace() const
        {
            return this->verticalSpace;
        }
    };
}

#endif

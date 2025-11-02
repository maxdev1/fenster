// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TREENODE
#define FENSTER_SERVER_COMPONENTS_TREENODE

#include "components/component.hpp"
#include "components/label.hpp"

namespace fensterserver
{
    class TreeNode : virtual public Component, virtual public TitledComponent
    {
        Label label;
        bool open = false;

    public:
        TreeNode();
        ~TreeNode() override = default;

        void update() override;
        void layout() override;

        void setTitleInternal(std::string title) override;
        std::string getTitle() override;

        Component* handleMouseEvent(MouseEvent& event) override;
    };
}


#endif

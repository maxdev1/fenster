// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TREE
#define FENSTER_SERVER_COMPONENTS_TREE

#include "components/component.hpp"

#include <libjson/json.hpp>

namespace fensterserver
{
    class TreeNode;

    class Tree : virtual public Component
    {
    public:
        Tree();
        ~Tree() override = default;

        void setModelFromJson(std::string& model);
        TreeNode* createNodeComponent(json::JsonNode& node);
    };
}

#endif

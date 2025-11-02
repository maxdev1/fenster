// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_TREE__
#define __WINDOWSERVER_COMPONENTS_TREE__

#include "components/component.hpp"

#include <libjson/json.hpp>

class tree_node_t;

class tree_t : virtual public component_t
{
public:
    tree_t();
    ~tree_t() override = default;

    void setModelFromJson(std::string& model);
    tree_node_t* createNodeComponent(g_json_node& node);
};

#endif

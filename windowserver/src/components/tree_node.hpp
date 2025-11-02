// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __WINDOWSERVER_COMPONENTS_TREE_NODE__
#define __WINDOWSERVER_COMPONENTS_TREE_NODE__

#include "components/component.hpp"
#include "components/label.hpp"

class tree_node_t : virtual public component_t, virtual public titled_component_t
{
    label_t label;
    bool open = false;

public:
    tree_node_t();
    ~tree_node_t() override = default;

    void update() override;
    void layout() override;

    void setTitleInternal(std::string title) override;
    std::string getTitle() override;

    component_t* handleMouseEvent(mouse_event_t& event) override;
};


#endif

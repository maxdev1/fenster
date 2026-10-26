// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "layout/flex_layout.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace fensterserver
{
void FlexLayout::setComponentInfo(Component* child, float grow, float shrink, int basis)
{
    flexInfo[child] = {std::isfinite(grow) ? std::max(0.f, grow) : 0.f,
        std::isfinite(shrink) ? std::max(0.f, shrink) : 0.f, std::max(-1, basis)};
}

void FlexLayout::layout()
{
    if(!component) return;
    const bool horizontal = orientation == fenster::Orientation::Horizontal;
    const auto bounds = component->getBounds();
    const int main = std::max(0, horizontal ? bounds.width - padding.left - padding.right : bounds.height - padding.top - padding.bottom);
    const int cross = std::max(0, horizontal ? bounds.height - padding.top - padding.bottom : bounds.width - padding.left - padding.right);
    struct Item { Component* child; double size, basis, grow, shrink; int min, max, cross; };
    std::vector<Item> items;
    int preferredMain = 0, preferredCross = 0;
    auto& children = component->acquireChildren();
    for(auto& ref : children)
    {
        auto child = ref.component;
        if(!child->isVisible()) continue;
        auto found = flexInfo.find(child);
        const FlexInfo info = found == flexInfo.end() ? FlexInfo{0, 0, -1} : found->second;
        auto pref = child->getEffectivePreferredSize(), min = child->getMinimumSize(), max = child->getMaximumSize();
        int lower = std::max(0, horizontal ? min.width : min.height);
        int upper = horizontal ? max.width : max.height;
        upper = upper < 0 ? 10000 : std::max(lower, upper);
        int basis = std::max(lower, std::min(upper, info.basis < 0 ? (horizontal ? pref.width : pref.height) : info.basis));
        int crossMin = std::max(0, horizontal ? min.height : min.width);
        int crossMax = horizontal ? max.height : max.width;
        int crossSize = std::max(crossMin, crossMax < 0 ? cross : std::min(cross, std::max(crossMin, crossMax)));
        items.push_back({child, double(basis), double(basis), info.grow, info.shrink, lower, upper, crossSize});
        preferredMain += basis;
        preferredCross = std::max(preferredCross, horizontal ? pref.height : pref.width);
    }
    const int gaps = items.empty() ? 0 : std::max(0, space) * (items.size() - 1);
    double remaining = main - gaps - preferredMain;
    // Redistribute space left by children that reach a constraint. Keep the
    // configured auto basis intact so later text/preferred-size changes reflow.
    for(size_t pass = 0; pass < items.size() && std::abs(remaining) > .001; ++pass)
    {
        double weight = 0;
        for(auto& item : items)
            if(remaining > 0 ? item.size < item.max : item.size > item.min)
                weight += remaining > 0 ? item.grow : item.shrink * item.basis;
        if(weight <= 0) break;
        double used = 0;
        for(auto& item : items)
        {
            if(remaining > 0 ? item.size >= item.max : item.size <= item.min) continue;
            double share = (remaining > 0 ? item.grow : item.shrink * item.basis) / weight * remaining;
            double size = std::max(double(item.min), std::min(double(item.max), item.size + share));
            used += size - item.size; item.size = size;
        }
        if(std::abs(used) < .001) break;
        remaining -= used;
    }
    int position = horizontal ? padding.left : padding.top, rounded = 0;
    double accumulated = 0;
    for(auto& item : items)
    {
        accumulated += item.size;
        int end = std::lround(accumulated), size = end - rounded; rounded = end;
        item.child->setBounds(horizontal ? fenster::Rectangle(position, padding.top, size, item.cross) :
            fenster::Rectangle(padding.left, position, item.cross, size));
        position += size + std::max(0, space);
    }
    component->releaseChildren();
    preferredMain += gaps;
    component->setPreferredSize(horizontal ? fenster::Dimension(preferredMain + padding.left + padding.right, preferredCross + padding.top + padding.bottom) :
        fenster::Dimension(preferredCross + padding.left + padding.right, preferredMain + padding.top + padding.bottom));
}
}

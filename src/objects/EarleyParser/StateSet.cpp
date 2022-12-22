//
// Created by michel on 22.12.22.
//

#include "StateSet.h"
#include <iostream>

void StateSet::addEarleyItem(const EarleyItem& earleyItem) {
    for (const auto& item : m_set) {
        if (item == earleyItem) {
            return;
        }
    }
    m_set.emplace_back(earleyItem);
}
void StateSet::addEarleyItemUnsafe(const EarleyItem& earleyItem) { m_set.emplace_back(earleyItem); }

void StateSet::print(std::ostream& out = std::cout) const {
    for (const auto& item : m_set) {
        item.print(out);
    }
}

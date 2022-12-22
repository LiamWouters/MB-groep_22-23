//
// Created by michel on 22.12.22.
//

#include "StateSet.h"

void StateSet::addEarleyItem(const EarleyItem& earleyItem) {
    for (const auto& item : m_set) {
        if (item == earleyItem) {
            return;
        }
    }
    m_set.emplace_back(earleyItem);
}

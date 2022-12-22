//
// Created by michel on 22.12.22.
//

#include "EarleyItem.h"

#include <utility>
EarleyItem::EarleyItem(Production production, unsigned int dot, unsigned int start)
    : m_production{std::move(production)}, m_dot{dot}, m_start{start} {}

bool EarleyItem::operator==(const EarleyItem& rhs) const {
    if (m_dot == rhs.m_dot && m_start == rhs.m_start && m_production == rhs.m_production) {
        return true;
    } else {
        return false;
    }
}

bool EarleyItem::isCompleted() const {
    if (m_dot == m_production.body.size()) {
        return true;
    } else {
        return false;
    }
}

std::string EarleyItem::getNextSymbol() const { return m_production.body[m_dot]; }

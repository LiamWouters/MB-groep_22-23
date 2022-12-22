//
// Created by michel on 22.12.22.
//

#include "EarleyItem.h"

#include <iomanip>
#include <iostream>
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

void EarleyItem::print(std::ostream& out = std::cout) const {
    out << std::left << std::setw(6) << m_production.head << std::setw(4) << "->" << std::setw(12);
    for (unsigned int i = 0; i < m_production.body.size(); i++) {
        if (m_dot == i) {
            out << "•";
        }
        std::string cur_to_print = m_production.body[i];
        if (cur_to_print == "\n") {
            out << "\\n";
        }
        if (cur_to_print == "\t") {
            out << "\\t";
        }
        if (cur_to_print == " ") {
            out << "space";
        }
        out << cur_to_print;
        out << " ";
    }
    out << "(" << m_start << ")" << std::endl;
}

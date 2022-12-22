//
// Created by michel on 22.12.22.
//

#include "EarleyItem.h"

#include <utility>
EarleyItem::EarleyItem(Production  production, unsigned int dot, unsigned int start): m_production{std::move(production)}, m_dot{dot}, m_start{start} {}

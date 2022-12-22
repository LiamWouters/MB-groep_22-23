//
// Created by michel on 22.12.22.
//

#include "EarleyParser.h"
EarleyParser::EarleyParser() {}

void EarleyParser::initChart() {
    // initialize first StateSet
    for (const auto& prod : m_grammar.p) {
        if (prod.head == m_grammar.s) {
            m_chart.emplace_back();
            m_chart[0].addEarleyItem({prod, 0, 0});
        }
    }
}

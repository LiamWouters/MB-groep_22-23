//
// Created by michel on 22.12.22.
//

#include "EarleyParser.h"
EarleyParser::EarleyParser() {}

void EarleyParser::initChart() {
    // initialize first StateSet
    for (const auto& prod : m_grammar.p) {
        // find productions with starting symbol as head
        if (prod.head == m_grammar.s) {
            // add production to chart
            m_chart.emplace_back();
            m_chart[0].addEarleyItem({prod, 0, 0});
        }
    }
}

void EarleyParser::predict(const std::string& next_symbol, unsigned int index_chart) {
    // find productions with next_symbol as head
    for (const auto& prod : m_grammar.p) {
        if (prod.head == next_symbol) {
            // add to chart
            m_chart[index_chart].addEarleyItem({prod, 0, index_chart});
        }
    }
}

void EarleyParser::scan(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set) {
    // compare next_symbol with input
    if (next_symbol == m_input[index_chart].content) {
        EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);
        // add StateSet to chart if needed (make chart array 1 bigger)
        if (m_chart.size() - 1 == index_chart) {
            m_chart.emplace_back();
        }
        // add cur EarleyItem in next StateSet with dot one place advanced
        m_chart[index_chart + 1].addEarleyItemUnsafe({cur_item.m_production, cur_item.m_dot + 1, cur_item.m_start});
    }
}

void EarleyParser::completion(unsigned int index_chart, unsigned int index_state_set) {
    EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);

    // look for productions to be completed
    for (auto& item : m_chart[cur_item.m_start].m_set) {
        if (cur_item.m_production.head == item.getNextSymbol()) {
            // complete
            m_chart[index_chart].addEarleyItem({item.m_production, item.m_dot + 1, item.m_start});
        }
    }
}

EarleyItem EarleyParser::getEarlyItem(unsigned int index_chart, unsigned int index_state_set) const {
    return m_chart[index_chart].m_set[index_state_set];
}
void EarleyParser::printChart(std::ostream& out = std::cout) const {
    for (unsigned int index_chart = 0; index_chart < m_chart.size(); index_chart++) {
        out << "=== " << index_chart << "===" << std::endl;
        m_chart[index_chart].print(out);
        out << std::endl;
    }
}

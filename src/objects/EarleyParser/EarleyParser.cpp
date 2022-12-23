//
// Created by michel on 22.12.22.
//

#include "EarleyParser.h"
#include "../../utilities/utilities.h"

EarleyParser::EarleyParser(const CFG& grammar) : m_grammar{grammar} { init(); }

void EarleyParser::initChart() {
    // initialize first StateSet
    for (const auto& prod : m_grammar.p) {
        // find productions with starting symbol as head
        if (prod.head == m_grammar.s) {
            // add production to chart
            m_chart.emplace_back();
            m_chart[0].addEarleyItemUnsafe({prod, 0, 0});
        }
    }
}

void EarleyParser::predict(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set) {
    // find productions with next_symbol as head
    for (const auto& prod : m_grammar.p) {
        if (prod.head == next_symbol) {
            // add to chart
            m_chart[index_chart].addEarleyItem({prod, 0, index_chart});

            // special completion to handle nullables
            if (isNullable(prod.head)) {
                // get item
                EarleyItem to_complete = getEarlyItem(index_chart, index_state_set);
                m_chart[index_chart].addEarleyItem(
                    {to_complete.m_production, to_complete.m_dot + 1, to_complete.m_start});
            }
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

void EarleyParser::complete(unsigned int index_chart, unsigned int index_state_set) {
    EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);

    // look for productions to be completed
    for (auto& item : m_chart[cur_item.m_start].m_set) {
        if (item.isDotAtEnd()) {
            continue;
        }
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

void EarleyParser::init() { m_nullables = m_grammar.findNullableVariables(); }

bool EarleyParser::isNullable(const std::string& variable) const { return stringInSet(variable, m_nullables); }

void EarleyParser::fillChart() {
    for (unsigned int index_chart = 0; index_chart < m_chart.size(); index_chart++) {
        for (unsigned int index_state_set = 0; index_state_set < m_chart[index_chart].m_set.size(); index_state_set++) {
            addItemToChart(index_chart, index_state_set);
        }
    }
}
void EarleyParser::addItemToChart(unsigned int index_chart, unsigned int index_state_set) {
    if (m_chart[index_chart].m_set[index_state_set].isDotAtEnd()) {
        complete(index_chart, index_state_set);
    } else {
        std::string next_symbol = m_chart[index_chart].m_set[index_state_set].getNextSymbol();
        if (m_grammar.isVariable(next_symbol)) {
            predict(next_symbol, index_chart, index_state_set);
        } else {
            scan(next_symbol, index_chart, index_state_set);
        }
    }
}

bool EarleyParser::validate(const std::vector<token>& input) {
    m_chart.clear();
    m_input = input;
    initChart();
    fillChart();
    return has_complete_parse();
}

bool EarleyParser::has_partial_parse(unsigned int index_chart) const {
    for (unsigned int index_state_set = 0; index_state_set < m_chart[index_chart].m_set.size(); index_state_set++) {
        EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);
        if (cur_item.m_production.head == m_grammar.s && cur_item.isDotAtEnd() && cur_item.m_start == 0) {
            return true;
        }
    }
    return false;
}

bool EarleyParser::has_complete_parse() const { return has_partial_parse(m_chart.size() - 1); }

unsigned int EarleyParser::get_index_last_partial_parse() const {
    unsigned int index_last_partial_parse = 0;
    for (unsigned int index_chart = 0; index_chart < m_chart.size(); index_chart++) {
        if (has_partial_parse(index_chart)) {
            index_last_partial_parse = index_chart;
        }
    }
    return index_last_partial_parse;
}

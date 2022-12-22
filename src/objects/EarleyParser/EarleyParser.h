//
// Created by michel on 22.12.22.
//

#ifndef MB_GROEP_22_23_EARLEYPARSER_H
#define MB_GROEP_22_23_EARLEYPARSER_H

#include "../CFG.h"
#include "../Token.h"
#include "StateSet.h"

class EarleyParser {
public:
    /// constructors
    EarleyParser();

    /// member functions
    void initChart(); // initializes chart
    void predict(const std::string& next_symbol, unsigned int index_chart);
    void scan(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set);
    void completion(unsigned int index_chart, unsigned int index_state_set);

    /// print functions
    void printChart(std::ostream& out) const;

    /// variables
    std::vector<StateSet> m_chart;
    CFG m_grammar;
    std::vector<token> m_input;

private:
    /// helper functions
    EarleyItem getEarlyItem(unsigned int index_chart, unsigned int index_state_set) const;
};

#endif // MB_GROEP_22_23_EARLEYPARSER_H

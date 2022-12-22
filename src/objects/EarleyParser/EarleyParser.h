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

    /// memberfunctions
    void initChart(); // initializes chart

    /// variables
    std::vector<StateSet> m_chart;
    CFG m_grammar;
    std::vector<token> m_input;
};

#endif // MB_GROEP_22_23_EARLEYPARSER_H

//
// Created by michel on 22.12.22.
//

#ifndef MB_GROEP_22_23_EARLEYPARSER_H
#define MB_GROEP_22_23_EARLEYPARSER_H

#include "StateSet.h"
#include "../CFG.h"

class EarleyParser {
public:
    std::vector<StateSet> m_chart;
    CFG grammar;
};

#endif // MB_GROEP_22_23_EARLEYPARSER_H

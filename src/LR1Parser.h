//
// Created by liam on 13/12/2022.
//

#ifndef MB_GROEP_22_23_LR1PARSER_H
#define MB_GROEP_22_23_LR1PARSER_H

#include "objects/CFG.h"

class LR1Parser {
private:
    CFG grammar; // not a pointer, it's a copy of the grammar given through the Constructor

public:
    LR1Parser(const CFG &grammar);

    void constructParseTable();

    bool parse(std::string input);
};


#endif //MB_GROEP_22_23_LR1PARSER_H

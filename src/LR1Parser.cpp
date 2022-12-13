//
// Created by liam on 13/12/2022.
//

#include <iostream>
#include "LR1Parser.h"

/*
 * CONSTRUCTORS
 */
LR1Parser::LR1Parser(const CFG &grammar) : grammar(CFG(grammar)) {}

///////////////////////////////////////////////////////////////////////
/*
 * PRIVATE FUNCTIONS
 */

///////////////////////////////////////////////////////////////////////
/*
 * PUBLIC FUNCTIONS
 */
void LR1Parser::constructParseTable() {
    /*
     * The input grammar may NOT already have a state called "S"
     */
    /// Augment grammar
    // add new starting state
    grammar.addVariable("S");
    grammar.addProduction(Production("S", {grammar.getStartState()}));
    grammar.setStartState("S");

    // create item sets

}

bool LR1Parser::parse(std::string input) {
    /// check if parse table is constructed (if not construct)

    /// parse
}
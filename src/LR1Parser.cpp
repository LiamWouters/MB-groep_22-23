//
// Created by liam on 13/12/2022.
//

#include <iostream>
#include <algorithm>
#include "LR1Parser.h"

/*
 * CONSTRUCTORS
 */
LR1Parser::LR1Parser(const CFG &grammar) : grammar(CFG(grammar)) {
    debugprint = false;
    constructParseTable();
}

///////////////////////////////////////////////////////////////////////
/*
 * PRIVATE FUNCTIONS
 */

void LR1Parser::createFirstSet() {
    std::vector<std::string> variables = grammar.getVariables();
    std::vector<std::string> terminals = grammar.getTerminals();
    /// MAY NOT WORK CORRECTLY IF GRAMMAR CONTAINS EPSILON TRANSITIONS
    for (std::string var : variables) {
        first.insert(std::make_pair(var, std::set<std::string>{})); // initialize set
        for (Production prod : grammar.getProductions()) {
            if (prod.head == var) {
                first[var].insert(prod.body[0]); // we will replace the variables later
            }
        }
    }
    // replace non-terminals
    for (std::string var : variables) {
        // apply production rules for each variable in set (until no more variables are added)
        int originalSetSize = 0;
        while (originalSetSize != first[var].size()) {
            originalSetSize = first[var].size();
            for (std::string symbol : first[var]) {
                if (std::find(variables.begin(), variables.end(), symbol) != variables.end()) {
                    // symbol == variable
                    for (Production prod : grammar.getProductions()) {
                        if (prod.head == symbol) {
                            first[var].insert(prod.body[0]);
                        }
                    }
                }
            }
        }

        // remove all variables in set
        for (std::string v : variables) {
            first[var].erase(v);
        }
    }
}

void LR1Parser::createFollowSet(int itemSetNumber) {
    std::vector<std::string> variables = grammar.getVariables();
    std::vector<std::string> terminals = grammar.getTerminals();
    /*
     * will hold the set of terminals that can appear immediately after a nonterminal
     * 3 possibilities: (B = nonterminal)
     *          -> X -> a B b   [b is added to FOLLOW(x, B)]            RULE 1
     *          -> X -> a B A   [FIRST(A) is added to FOLLOW(x, B)]     RULE 2
     *          -> A -> a B     [FOLLOW(x, A) is added to FOLLOW(x, B)] RULE 3
     */
    //initialize
    for (std::string variable : grammar.getVariables()) {
        follow[{itemSetNumber,variable}] = std::set<std::string>{"EOS"}; /// NOT SURE IF YOU ALWAYS HAVE EOS, MAY BE WRONG !!!!!!!!!!!!!!!!!!!!!!! CHECK
    }
    /// Rule 1&2:
    for (std::pair<Production, std::string> item : parserItemSets[itemSetNumber]) {
        // check production body for Variables, add next variable to the follow set for that variable
        for (int i = 0; i < item.first.body.size()-1; i++) { // we don't check the last symbol because it doesn't have a follow
            auto symbol = item.first.body[i];
            auto next = item.first.body[i+1];
            if (std::find(variables.begin(), variables.end(), symbol) != variables.end()) {
                // if variable, check next symbol, if that symbol is a variable, add all the FIRST(nextsymb) terminals
                // otherwise if next symbol = terminal add that terminal
                if (std::find(variables.begin(), variables.end(), next) != variables.end()) {
                    for (std::string terminal : first[next]) {
                        follow[{itemSetNumber,symbol}].insert(terminal);
                    }
                }
                else {
                    follow[{itemSetNumber,symbol}].insert(next);
                }
            }
        }
    }
    /// Rule 3: (now that Rule 1 and 2 are fully done we can apply this rule without missing anything)
    for (std::pair<Production, std::string> item : parserItemSets[itemSetNumber]) {
        // loop over all productions again
        // check if production is of the form: A -> ... B
        std::string lastSymbol = item.first.body[item.first.body.size()-1];
        if ((item.first.head != lastSymbol) && (std::find(variables.begin(), variables.end(), lastSymbol) != variables.end())) {
            // add FOLLOW of head to FOLLOW of lastSymbol
            for (std::string term : follow[{itemSetNumber, item.first.head}]) {
                follow[{itemSetNumber, lastSymbol}].insert(term);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////
/*
 * PUBLIC FUNCTIONS
 */
void LR1Parser::constructParseTable() {
    if (debugprint) {
        std::cout << "start grammar: " << std::endl;
        grammar.print();
        std::cout << "________________________________________" << std::endl;
    }
    /*
     * The input grammar may NOT already have a state called "S"
     * The input grammar may NOT already have a terminal "*"
     */
    /// Augment grammar
    // add new starting state
    grammar.addVariable("S");
    grammar.addProduction(Production("S", {grammar.getStartState()}));
    grammar.setStartState("S");

    if (debugprint) {
        std::cout << "Augmented grammar: " << std::endl;
        grammar.print();
        std::cout << "________________________________________" << std::endl;
    }
    /// create item sets
    /// item set 0:
    // create item set 0 for LR(0) (no lookaheads)
    std::vector<std::pair<Production, std::string>> itemSet0;
    for (Production prod : grammar.getProductions()) {
        // augment body (add '*' to indicate current parsing position)
        prod.body.insert(prod.body.begin(), "*");
        itemSet0.emplace_back(std::make_pair(prod, "")); // lookahead is empty
    }
    parserItemSets.insert(std::make_pair(0, itemSet0));

    if (debugprint) {
        std::cout << "(current) Item Set 0 : " << std::endl;
        for (std::pair<Production, std::string> item : itemSet0) {
            std::cout << "\t [" << item.first.head << " -> ";
            for (std::string symbol : item.first.body) {
                std::cout << symbol;
            }
            std::cout << "]" << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }

    // create FIRST set
    createFirstSet();

    if (debugprint) {
        std::cout << "First Set: " << std::endl;
        for (std::string variable : grammar.getVariables()) {
            std::cout << "\t FIRST(" << variable << ") = { ";
            for (std::string terminal : first[variable]) {
                std::cout << "\'" << terminal << "\' ";
            }
            std::cout << "}" << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }

    // using the incomplete item set 0, create FOLLOW set
    createFollowSet(0);

    if (debugprint) {
        std::cout << "Follow Set: " << std::endl;
        for (std::string variable : grammar.getVariables()) {
            std::cout << "\t FOLLOW(0," << variable << ") = { ";
            for (std::string terminal : follow[{0,variable}]) {
                std::cout << "\'" << terminal << "\' ";
            }
            std::cout << " }" << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }
    // create complete item set 0!


}

bool LR1Parser::parse(std::string input) {
    /// check if parse table is constructed (if not construct)

    /// parse

    return false;
}
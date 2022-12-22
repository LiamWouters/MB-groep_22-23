//
// Created by liam on 13/12/2022.
//

#ifndef MB_GROEP_22_23_LR1PARSER_H
#define MB_GROEP_22_23_LR1PARSER_H

#include <vector>
#include <stack>
#include <sstream>
#include "CFG.h"
#include "Production.h"

class LR1Parser {
private:
    bool debugprint;
    std::stringstream printbuffer;

    CFG grammar; // not a pointer, it's a copy of the grammar given through the Constructor

    std::map<int, std::set<std::pair<Production, std::string>>> parserItemSets; /// item set == state of parser
    // form: {#item_set, {[production, lookahead], ...}}
    std::map<std::string, std::set<std::string>> first;
    // first set:   form: {variable, {terminal1, terminal2, ...}}
    //              the set of terminals which can appear as the first element of any chain of rules matching nonterminal "variable"
    std::map<std::pair<int, std::string>, int> gotoTable;
    // GOTO table:  form: {(parentItemSetNumber, recognizedSymbol), parserItemSetNumber}
    //              giving a pair of an (parent)ITEMSETNUMBER and a SYMBOL (symbol = symbol that is recognized after * (symbol that is getting parsed that leads to the marker shifting (recognizing the symbol)))
    //              returns the item set (STATE of parser) that the parser will go to (after recognizing symbol)
    std::map<std::pair<int, std::string>, std::vector<std::string>> actionTable;
    // ACTION table:  form: {(parentItemSetNumber, recognizedSymbol), (action, state)}
    //              same input as GOTO table
    //              returns action, state pair
    //              vb: "accept" | "shift", "4"| "reduce", "A", "->", "b" (reduce A->b, A is head, b is body)

    std::stack<std::string> parserStack;

    ////////////////////////////////////////////////
    bool testUnique(const std::set<std::pair<Production, std::string>>* itemset1, const std::set<std::pair<Production, std::string>>* itemset2);
    std::set<std::pair<Production, std::string>> GOTO(const std::set<std::pair<Production, std::string>>* itemSet, const std::string symbol);
    void computeGOTO();
    void createGOTOTable();
    void createACTIONTable();
    void generateClosure(std::set<std::pair<Production, std::string>>& itemSet); // generates the closure of the given item set
    void createFirstSet();
    std::string getParsedSymbol(const std::pair<Production, std::string> item);
    bool augmentRule(Production& rule);
    bool shiftMarker(std::pair<Production, std::string>& item);
public:
    LR1Parser(const CFG &grammar, const bool debugprint = false);
    const std::stringstream &getPrintbuffer() const;

    void constructParseTable();

    bool parse(std::vector<std::string> input);
};


#endif //MB_GROEP_22_23_LR1PARSER_H

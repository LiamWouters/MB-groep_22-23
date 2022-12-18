//
// Created by liam on 13/12/2022.
//

#ifndef MB_GROEP_22_23_LR1PARSER_H
#define MB_GROEP_22_23_LR1PARSER_H

#include <vector>
#include "objects/CFG.h"
#include "objects/Production.h"

class LR1Parser {
    /*
     * TO DO:   * check of bij de follow set altijd alles 'EOS' moet bevatten of niet
     */
private:
    bool debugprint;
    CFG grammar; // not a pointer, it's a copy of the grammar given through the Constructor

    std::map<int, std::vector<std::pair<Production, std::string>>> parserItemSets;
    // form: {#item_set, {[production, lookahead], ...}}
    std::map<std::string, std::set<std::string>> first; // used alongside the follow set to determine lookahead (mentioned above)
    // first set:   form: {variable, {terminal1, terminal2, ...}}
    //              the set of terminals which can appear as the first element of any chain of rules matching nonterminal "variable"
    std::map<std::pair<int, std::string>, std::set<std::string>> follow; // used alongside the first set to determine lookahead (mentioned above)
    // follow set:  form: {(#item_set, variable), {terminal1, terminal2, ...}}
    //              the set of terminals that can appear immediately after the given variable

    ////////////////////////////////////////////////
    void createFirstSet();
    void createFollowSet(int itemSetNumber);
public:
    LR1Parser(const CFG &grammar);

    void constructParseTable();

    bool parse(std::string input);
};


#endif //MB_GROEP_22_23_LR1PARSER_H

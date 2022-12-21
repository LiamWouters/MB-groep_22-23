//
// Created by liam on 13/12/2022.
//

#include <iostream>
#include <algorithm>
#include <queue>
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

void LR1Parser::createGOTOTable() {
    /*
     * fill in the goto table
     * test every state, symbol pair if it creates an existing state
     */
    for (int parentStateNumber = 0; parentStateNumber < parserItemSets.size(); parentStateNumber++) {
        auto terms = grammar.getTerminals();
        std::vector<std::string> allSymbols(grammar.getVariables());
        allSymbols.insert(allSymbols.begin(), terms.begin(), terms.end());

        for (std::string symbol : allSymbols) {
            if (symbol == "S") { continue; } // state is not in original grammar, will not be in goto table
            auto gotoState = GOTO(&parserItemSets[parentStateNumber], symbol);

            for (int existingStateNum = 0; existingStateNum < parserItemSets.size(); existingStateNum++) {
                if (!testUnique(&gotoState, &parserItemSets[existingStateNum])) {
                    // goto state == an existing state, add to GOTOtable
                    gotoTable[std::make_pair(parentStateNumber, symbol)] = existingStateNum;
                }
            }
        }
    }
}

void LR1Parser::createACTIONTable() {
    std::vector<std::string> terminals = grammar.getTerminals();
    /*
     * RULES: 1) if item i of item set x is of the form [A->... * a ..., b] AND goto(x, a) = item set k. (a == terminal)
     *           THEN: action(x, a) = shift k
     *        2) if item i of item set x is of the form [S'->S*, EOS] (S' is augmented start state, S is original start state)
     *           THEN: action(x, a) = accept 0
     *        3) if item i of item set x is of the form [A->... *, a]
     *           THEN: action(x, a) = reduce A->...
     */

    for (auto itemset : parserItemSets) {
        for (auto item : itemset.second) {
            std::string parsedSymbol = getParsedSymbol(item);
            /// RULE 1:
            if (std::find(terminals.begin(), terminals.end(), parsedSymbol) != terminals.end()) {
                // parsed symbol == terminal
                if (gotoTable.count(std::make_pair(itemset.first, parsedSymbol)) > 0) { // exists
                    int gotoStateNumber = gotoTable[std::make_pair(itemset.first, parsedSymbol)];
                    actionTable[std::make_pair(itemset.first, parsedSymbol)] = {"shift", std::to_string(gotoStateNumber)};
                }
            }
            /// RULE 2:
            else if (item.first.head == "S" && parsedSymbol == "EOS" && item.second == "EOS") {
                // "S" is added start state
                // parsed symbol = EOS (last symbol is *)
                // lookahead = EOS
                actionTable[std::make_pair(itemset.first, parsedSymbol)] = {"accept"};
            }
            /// RULE 3:
            else if (parsedSymbol == "EOS"){
                actionTable[std::make_pair(itemset.first, item.second)] = {"reduce", item.first.head, "->"};
                for (std::string body : item.first.body) {
                    actionTable[std::make_pair(itemset.first, item.second)].emplace_back(body);
                }
            }
        }
    }
}

bool LR1Parser::testUnique(const std::set<std::pair<Production, std::string>>* itemset1, const std::set<std::pair<Production, std::string>>* itemset2) {
    /*
     * test if all items in sets are similar
     * return:  false -> the sets are not unique (they are the same)
     *          true  -> they are unique (they are not the same)
     */
    if ((itemset1->empty() && !itemset2->empty()) or (!itemset1->empty() && itemset2->empty())) {
        return true; // sets are unique (one is empty)
    }
    if (itemset1->empty() && itemset2->empty()) {
        return false; // NOT unique (both empty)
    }

    for (auto item1 : *itemset1) {
        bool item1Found = false; // item1 found in set 2
        for (auto item2 : *itemset2) {
            if ((item1.first.head == item2.first.head) && (item1.first.body == item2.first.body) && (item1.second == item2.second)) {
                item1Found = true; // head and body of production are the same, AND lookahead is the same
                break; // the item is found, no need to check the rest
            }
        }
        if (!item1Found) {
            return true; // an item in itemset1 was not found in itemset2, so they are unique
        }
    }
    return false; // there were no unique items found in the sets, they are the same (not unique)
}

std::set<std::pair<Production, std::string>> LR1Parser::GOTO(const std::set<std::pair<Production, std::string>>* itemSet, const std::string symbol) {
    std::set<std::pair<Production, std::string>> newItemSet;
    // include items that recognize symbol (and shift their marker)
    for (std::pair<Production, std::string> item : *itemSet) {
        if (symbol == getParsedSymbol(item)) {
            // ensure that given symbol follows the * (is being parsed)
            shiftMarker(item);
            newItemSet.insert(item);
        }
    }
    // produce closure
    generateClosure(newItemSet); // missing lookaheads-
    // put in lookaheads (using follow)
    std::set<std::pair<Production, std::string>> newItemSetLookaheads;
    for (std::pair<Production, std::string> item : newItemSet) {
        if (item.second == "") {
            // doesn't have lookahead yet
            for (std::string term : follow[item.first.head]) {
                newItemSetLookaheads.insert(std::make_pair(item.first, term));
            }
        }
        else {
            // if it does have a lookahead, it wasn't created in the closure but is one of the old rules (with marker shifted)
            // just re-add it
            newItemSetLookaheads.insert(item);
        }
    }

    return newItemSetLookaheads;
}

void LR1Parser::computeGOTO() {
    std::queue<std::set<std::pair<Production, std::string>>> itemSetQueue;
    // queue keeps the item sets that need to be checked if they can create new item sets
    itemSetQueue.emplace(parserItemSets[0]);
    parserItemSets.clear(); // remove itemset0 from parserItemSets (re-added later)
    while (!itemSetQueue.empty()) {
        std::set<std::pair<Production, std::string>> itemSet = itemSetQueue.front();
        itemSetQueue.pop();

        for (std::string var: grammar.getVariables()) {
            auto newSet = GOTO(&itemSet, var);
            // TEST emptyness and uniqueness before adding
            if (newSet.empty()) { continue; } // check if empty
            bool unique = true;
            for (auto parserItemSet : parserItemSets) {
                if (!testUnique(&parserItemSet.second, &newSet)) {
                    // test if newSet does not exist anywhere in already existing item sets
                    unique = false; // found equivalent item set
                    break;
                }
            }
            if (!unique) { continue; } // don't add if its not unique
            itemSetQueue.emplace(newSet);
        }
        for (std::string ter: grammar.getTerminals()) {
            auto newSet = GOTO(&itemSet, ter);
            // TEST emptyness and uniqueness before adding
            if (newSet.empty()) { continue; } // check if empty
            bool unique = true;
            for (auto parserItemSet : parserItemSets) {
                if (!testUnique(&parserItemSet.second, &newSet)) {
                    // test if newSet does not exist anywhere in already existing item sets
                    unique = false; // found equivalent item set
                    break;
                }
            }
            if (!unique) { continue; } // don't add if its not unique
            itemSetQueue.emplace(newSet);
        }

        /*
         * do a final check of uniqueness before adding itemSet to parserItemSets
         * Why? -> we check uniqueness of itemSet before adding it to the Queue, but we only check with parserItemSets.
         *         if there was an item set in the queue (that is the same) already in the queue, it would not get checked
         */
        // add new states (gotos/ itemsets) to the parseritemsets
        bool unique = true;
        for (auto parserItemSet : parserItemSets) {
            if (!testUnique(&parserItemSet.second, &itemSet)) {
                // test if newSet does not exist anywhere in already existing item sets
                unique = false; // found equivalent item set
                break;
            }
        }
        if (unique) { // add if unique
            parserItemSets.insert(std::make_pair(parserItemSets.size(), itemSet));
        }
    }
}

void LR1Parser::generateClosure(std::set<std::pair<Production, std::string>> &itemSet) {
    std::vector<std::string> variables = grammar.getVariables();
    /// MAY NOT WORK CORRECTLY IF GRAMMAR CONTAINS EPSILON TRANSITIONS
    /*
     * RULES: all production rules for each nonterminal following a "*" must be included into the item set, until all of those nonterminals are dealt with
     * (for item set 0 we begin with the start rule)
     * (no lookahead symbol is added for the new rules)
     */
    int ogSize = itemSet.size();

    for (std::pair<Production, std::string> item : itemSet) {
        std::string nextSymbol = getParsedSymbol(item);
        if (std::find(variables.begin(), variables.end(), nextSymbol) != variables.end()) {
            // next symbol is a variable
            // add its rules to the itemSet
            for (Production prod : grammar.getProductions()) {
                if (prod.head != nextSymbol) {continue;}
                augmentRule(prod);
                itemSet.insert(std::make_pair(prod, "")); /// NO LOOKAHEAD ADDED (calculated later)
            }
        }
    }

    if (ogSize != itemSet.size()) {
        // new rules were added
        // -> need to be checked for other variables after their "*"
        generateClosure(itemSet); // recursion
    }
}

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

std::map<std::string, std::set<std::string>> LR1Parser::createFollowSet(const std::set<std::pair<Production, std::string>>* itemSet) {
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
    std::map<std::string, std::set<std::string>> follow;
    for (std::string variable : grammar.getVariables()) {
        follow[variable] = std::set<std::string>{"EOS"}; /// NOT SURE IF YOU ALWAYS HAVE EOS, MAY BE WRONG !!!!!!!!!!!!!!!!!!!!!!! CHECK
    }
    /// Rule 1&2:
    for (std::pair<Production, std::string> item : *itemSet) {
        // check production body for Variables, add next variable to the follow set for that variable
        for (int i = 0; i < item.first.body.size()-1; i++) { // we don't check the last symbol because it doesn't have a follow
            auto symbol = item.first.body[i];
            auto next = item.first.body[i+1];
            if (std::find(variables.begin(), variables.end(), symbol) != variables.end()) {
                // if variable, check next symbol, if that symbol is a variable, add all the FIRST(nextsymb) terminals
                // otherwise if next symbol = terminal add that terminal
                if (std::find(variables.begin(), variables.end(), next) != variables.end()) {
                    for (std::string terminal : first[next]) {
                        follow[symbol].insert(terminal);
                    }
                }
                else {
                    if (std::find(terminals.begin(), terminals.end(), next) != terminals.end()) {
                        follow[symbol].insert(next); // must be a terminal (test because it could add "*" otherwise)
                    }
                }
            }
        }
    }
    /// Rule 3: (now that Rule 1 and 2 are fully done we can apply this rule without missing anything)
    for (std::pair<Production, std::string> item : *itemSet) {
        // loop over all productions again
        // check if production is of the form: A -> ... B
        std::string lastSymbol = item.first.body[item.first.body.size()-1];
        if ((item.first.head != lastSymbol) && (std::find(variables.begin(), variables.end(), lastSymbol) != variables.end())) {
            // add FOLLOW of head to FOLLOW of lastSymbol
            for (std::string term : follow[item.first.head]) {
                follow[lastSymbol].insert(term);
            }
        }
    }
    return follow;
}

std::string LR1Parser::getParsedSymbol(const std::pair<Production, std::string> item) {
    auto it = std::find(item.first.body.begin(), item.first.body.end(), "*");
    if (it >= item.first.body.end()-1) {return "EOS";} // no star OR star is the last symbol (no parsed symbol)
    else {return *(it+1);}
}

bool LR1Parser::shiftMarker(std::pair<Production, std::string>& item) {
    /*
     * Shift parser position in item
     */
    /// check if its already at the end
    if (item.first.body[item.first.body.size()-1] == "*") {
        return false;
    }
    /// else move the parsing position
    for (int i = 0; i < item.first.body.size()-1; i++) {
        if (item.first.body[i] == "*") {
            item.first.body.insert(item.first.body.begin() + i+2, "*");
            item.first.body.erase(item.first.body.begin() + i);
            return true;
        }
    }
    return false; // no marker found
}

bool LR1Parser::augmentRule(Production& rule) {
    // SHOULD ONLY BE USED IF THE RULE DOES NOT CONTAIN A "*" YET (in the beginning of adding the rule to item set 0)
    // augment body (add '*' to indicate current parsing position)
    if (std::find(rule.body.begin(), rule.body.end(), "*") != rule.body.end()) {
        // contains "*" already
        return false;
    }
    rule.body.insert(rule.body.begin(), "*");
    return true;
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
    /// source: "https://en.wikipedia.org/wiki/Canonical_LR_parser"
    /// the goto method: source: "https://www.eecis.udel.edu/~cavazos/cisc672-fall08/lectures/Lecture-10.pdf"
    // create item set 0, then itemset1 = goto(i0, x), ...
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
    /// calculate first set
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
    /// compute item set 0: (item set 0 == closure(s))
    std::set<std::pair<Production, std::string>> itemSet0;
    for (Production prod : grammar.getProductions()) {
        if (prod.head == grammar.getStartState()) {
            // augment body (add '*' to indicate current parsing position)
            augmentRule(prod);
            itemSet0.insert(std::make_pair(prod, "")); // lookahead is empty
        }
    }
    generateClosure(itemSet0); // incomplete (missing lookaheads, instead of FIRST(&a) [slide 7] we use FIRST and FOLLOW table)
    parserItemSets.insert(std::make_pair(0, itemSet0));

    if (debugprint) {
        std::cout << "(current) Item Set 0 : (closure from start rule, lookahead missing (LR(0)))" << std::endl;
        for (std::pair<Production, std::string> item : itemSet0) {
            std::cout << "\t [" << item.first.head << " -> ";
            for (std::string symbol : item.first.body) {
                std::cout << symbol;
            }
            if (item.second != "") {
                std::cout << ",\t" << item.second;
            }
            else {
                std::cout << ",\tNONE";
            }
            std::cout << "]" << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }

    // using the incomplete item set 0, create FOLLOW set
    follow = createFollowSet(&itemSet0);

    if (debugprint) {
        std::cout << "Follow Set: " << std::endl;
        for (std::string variable : grammar.getVariables()) {
            std::cout << "\t FOLLOW(0," << variable << ") = { ";
            for (std::string terminal : follow[variable]) {
                std::cout << "\'" << terminal << "\' ";
            }
            std::cout << " }" << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }
    // Finish item set 0!
    // find lookaheads using first and follow set
    // for each follow terminal create an item with that lookahead
    itemSet0.clear();
    for (std::pair<Production, std::string> item : parserItemSets[0]) {
        for (std::string term : follow[item.first.head]) {
            itemSet0.insert(std::make_pair(item.first, term));
        }
    }
    parserItemSets[0] = itemSet0;

    if (debugprint) {
        std::cout << "(FINAL) Item Set 0 : " << std::endl;
        for (std::pair<Production, std::string> item : parserItemSets[0]) {
            std::cout << "\t [" << item.first.head << " -> ";
            for (std::string symbol : item.first.body) {
                std::cout << symbol;
            }
            if (item.second != "") {
                std::cout << ",\t" << item.second;
            }
            else {
                std::cout << ",\tNONE";
            }
            std::cout << "]" << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }

    /// compute GOTOs
    computeGOTO();

    if (debugprint) {
        std::cout << "ALL ITEM SETS: " << std::endl;
        for (int i = 0; i<parserItemSets.size(); i++) {
            std::cout << "Item Set " << i << " : " << std::endl;
            for (std::pair<Production, std::string> item: parserItemSets[i]) {
                std::cout << "\t [" << item.first.head << " -> ";
                for (std::string symbol: item.first.body) {
                    std::cout << symbol;
                }
                if (item.second != "") {
                    std::cout << ",\t" << item.second;
                } else {
                    std::cout << ",\tNONE";
                }
                std::cout << "]" << std::endl;
            }
            std::cout << "________________________________________" << std::endl;
        }
    }

    /// fill GOTO table
    createGOTOTable();

    if (debugprint) {
        std::cout << "GOTO TABLE: " << std::endl;
        for (auto tableEntry : gotoTable) {
            std::cout << "\t GOTO(" << tableEntry.first.first << ", " << tableEntry.first.second << ") = " << tableEntry.second << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }

    /// fill ACTION table
    createACTIONTable();

    if (debugprint) {
        std::cout << "ACTION TABLE: " << std::endl;
        for (auto tableEntry : actionTable) {
            std::cout << "\t ACTION(" << tableEntry.first.first << ", " << tableEntry.first.second << ") = ";
            for (std::string b : tableEntry.second) {
                std::cout << b;
            }
            std::cout << std::endl;
        }
        std::cout << "________________________________________" << std::endl;
    }
}

bool LR1Parser::parse(std::string input) {
    /// check if parse table is constructed (if not construct)

    /// parse

    return false;
}
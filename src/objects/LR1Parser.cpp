//
// Created by liam on 13/12/2022.
//
#include "../../lib/nlohmann-json/json.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <queue>
#include <fstream>
#include "LR1Parser.h"

/*
 * CONSTRUCTORS
 */
LR1Parser::LR1Parser(const CFG &grammar, const bool debugprint) : grammar(grammar), debugprint(debugprint) {
    // normal constructor
    printbuffer.str(std::string());
    constructParseTable();
}

LR1Parser::LR1Parser(const std::string &fileLocation, const CFG &grammar, const bool debugprint) : grammar(grammar), debugprint(debugprint) {
    // load constructor
    printbuffer.str(std::string());
    // NOT CONSTRUCTING PARSE TABLE
    // we do need to augment the grammar
    augmentGrammar();

    /// load parser
    std::ifstream input(fileLocation);
    nlohmann::json j;
    input >> j;

    // fill parserItemSets
    for (int i = 0; i < j["parserItemSets"].size(); i++) {
        std::set<std::pair<Production, std::string>> itemSet;
        for (int item = 0; item < j["parserItemSets"][i]["itemSet"].size(); item++) {
            Production prod(j["parserItemSets"][i]["itemSet"][item]["head"], j["parserItemSets"][i]["itemSet"][item]["body"].get<std::vector<std::string>>());
            std::string lookahead = j["parserItemSets"][i]["itemSet"][item]["lookahead"];
            std::pair<Production, std::string> itemObject = std::make_pair(prod, lookahead);
            itemSet.insert(itemObject);
        }

        parserItemSets.insert(std::make_pair(j["parserItemSets"][i]["itemSetNumber"], itemSet));
    }
    // fill gotoTable
    for (int i = 0; i < j["gotoTable"].size(); i++) {
        std::pair<int, std::string> key = std::make_pair(j["gotoTable"][i]["key"]["parentItemSetNumber"], j["gotoTable"][i]["key"]["recognizedSymbol"]);
        gotoTable[key] = j["gotoTable"][i]["gotoSetNumber"];
    }
    // fill actionTable
    for (int i = 0; i < j["actionTable"].size(); i++) {
        std::pair<int, std::string> key = std::make_pair(j["actionTable"][i]["key"]["parentItemSetNumber"], j["actionTable"][i]["key"]["recognizedSymbol"]);
        std::vector<std::string> action = j["actionTable"][i]["action"].get<std::vector<std::string>>();
        actionTable[key] = action;
    }

    // debug print
    if (debugprint) {
        printbuffer << "ALL ITEM SETS:" << std::endl;
        for (int i = 0; i<parserItemSets.size(); i++) {
            printbuffer << "Item Set " << i << " :" << std::endl;
            for (std::pair<Production, std::string> item: parserItemSets[i]) {
                printbuffer << "  [" << item.first.head << " -> ";
                for (std::string symbol: item.first.body) {
                    printbuffer << symbol;
                }
                if (item.second != "") {
                    printbuffer << ", " << item.second;
                } else {
                    printbuffer << ", NONE";
                }
                printbuffer << "]" << std::endl;
            }
            printbuffer << "________________________________________" << std::endl;
        }
        ///+++///
        printbuffer << "GOTO TABLE:" << std::endl;
        for (auto tableEntry : gotoTable) {
            printbuffer << "  GOTO(" << tableEntry.first.first << ", " << tableEntry.first.second << ") = " << tableEntry.second << std::endl;
        }
        printbuffer << "________________________________________" << std::endl;
        ///+++///
        printbuffer << "ACTION TABLE:" << std::endl;
        for (auto tableEntry : actionTable) {
            printbuffer << "  ACTION(" << tableEntry.first.first << ", " << tableEntry.first.second << ") = ";
            for (std::string b : tableEntry.second) {
                printbuffer << b;
            }
            printbuffer << std::endl;
        }
        printbuffer << "________________________________________" << std::endl;
    }
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
    generateClosure(newItemSet);

    return newItemSet;
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
    std::vector<std::string> terminals = grammar.getTerminals();
    /// MAY NOT WORK CORRECTLY IF GRAMMAR CONTAINS EPSILON TRANSITIONS
    /*
     * RULES: all production rules for each nonterminal following a "*" must be included into the item set, until all of those nonterminals are dealt with
     * (for item set 0 we begin with the start rule)
     */
    int ogSize = itemSet.size();
    auto ogItemSet = itemSet;

    for (std::pair<Production, std::string> item : ogItemSet) {
        std::string parsedSymbol = getParsedSymbol(item);
        if (std::find(variables.begin(), variables.end(), parsedSymbol) != variables.end()) {
            // next symbol is a variable
            // add its rules to the itemSet

            // find lookaheads for possible new item sets
            std::vector<std::string> lookaheads = {};
            auto markerIt = std::find(item.first.body.begin(), item.first.body.end(), "*");
            if (markerIt+2 != item.first.body.end()) {
                // parsed symbol isn't the final symbol in the production
                std::string symbolAfterParsed = *(markerIt+2);
                if (std::find(terminals.begin(), terminals.end(), symbolAfterParsed) != terminals.end()) {
                    // is terminal
                    lookaheads.emplace_back(symbolAfterParsed);
                }
                else {
                    // is variable
                    for (auto f : first[symbolAfterParsed]) {
                        lookaheads.emplace_back(f);
                    }
                }
            }
            else {
                // parsed symbol IS the final symbol in the production
                std::string originalLookahead = item.second;
                lookaheads.emplace_back(originalLookahead);
            }

            for (Production prod : grammar.getProductions()) {
                if (prod.head != parsedSymbol) {continue;}
                augmentRule(prod);

                for (auto l : lookaheads) {
                    itemSet.insert(std::make_pair(prod, l));
                }
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
    // initialize first for all variables
    /*
    for (std::string var : variables) {
        first.insert(std::make_pair(var, std::set<std::string>{})); // initialize set
        for (Production prod : grammar.getProductions()) {
            if (prod.head == var) {
                if (prod.body.size() == 0) {
                    first[var].insert(""); // EPSILON
                }
                else {
                    first[var].insert(prod.body[0]); // we will replace the variables later
                }
            }
        }
    }
    // replace non-terminals
    for (std::string var : variables) {
        // apply production rules for each variable in set (until no more variables are added)
        int originalSetSize = 0;
        while (originalSetSize != first[var].size()) {
            originalSetSize = first[var].size();
            auto firstVarCopy = first[var];
            for (std::string symbol : firstVarCopy) {
                if (std::find(variables.begin(), variables.end(), symbol) != variables.end()) {
                    // symbol == variable
                    for (Production prod : grammar.getProductions()) {
                        if (prod.head == symbol && prod.body.size() > 0) {
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
     */
    int setChanged = true;
    while (setChanged) {
        setChanged = false;
        for (Production p : grammar.getProductions()) {
            int originalCount = -1; // -1 for if the set doesn't exist yet (count == 0)
            if (first.count(p.head) != 0) {
                originalCount = first[p.head].size();
            }
            if (p.body.size() > 0) {
                // normal production
                /// add first of the first symbol of the production (if variable), otherwise the symbol itself
                if (std::find(variables.begin(), variables.end(), p.body[0]) == variables.end()) {
                    // p.body[0] is terminal
                    if (first.count(p.head) == 0) { // add to first[head]
                        first[p.head] = {p.body[0]};
                    }
                    else {
                        first[p.head].insert(p.body[0]);
                    }
                }
                else {
                    // p.body[0] is variable
                    std::set<std::string> firstOfVar = first[p.body[0]];
                    firstOfVar.erase(""); // remove epsilon
                    if (first.count(p.head) == 0) { // add to first[head]
                        first[p.head] = firstOfVar;
                    }
                    else {
                        first[p.head].insert(firstOfVar.begin(), firstOfVar.end());
                    }
                }

                /// now all the other symbols in the production
                for (int i = 0; i<p.body.size()-1; i++) {
                    // while first[p.body[i]] contains epsilon,
                    if (std::find(variables.begin(), variables.end(), p.body[i]) == variables.end()) {break;} // not variable
                    if (std::find(first[p.body[i]].begin(), first[p.body[i]].end(), "") == first[p.body[i]].end()) {
                        // does NOT contain epsilon
                        break;
                    }
                    else {
                        // does contain epsilon
                        // add first[p.body[i+1]] to first[p.head]
                        if (std::find(variables.begin(), variables.end(), p.body[i+1]) == variables.end()) {
                            // p.body[i+1] is terminal
                            if (first.count(p.head) == 0) { // add to first[head]
                                first[p.head] = {p.body[i+1]};
                            }
                            else {
                                first[p.head].insert(p.body[i+1]);
                            }
                        }
                        else {
                            // p.body[i+1] is variable
                            std::set<std::string> firstOfVar = first[p.body[i+1]];
                            firstOfVar.erase(""); // remove epsilon
                            if (first.count(p.head) == 0) { // add to first[head]
                                first[p.head] = firstOfVar;
                            }
                            else {
                                first[p.head].insert(firstOfVar.begin(), firstOfVar.end());
                            }
                        }
                    }
                }
            }
            else {
                // epsilon production
                if (first.count(p.head) == 0) { // add to first[head]
                    first[p.head] = {""};
                }
                else {
                    first[p.head].insert("");
                }
            }
            int newCount = 0;
            if (first.count(p.head) != 0) {
                newCount = first[p.head].size();
            }
            if (newCount != originalCount) {setChanged = true;}
        }
    }
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

void LR1Parser::constructParseTable() {
    /*
     * The input grammar may NOT already have a state called "S"
     * The input grammar may NOT already have a terminal "*"
     * Epsilon = ""
     */
    /// source: "https://en.wikipedia.org/wiki/Canonical_LR_parser"
    /// the goto method: source: "https://www.eecis.udel.edu/~cavazos/cisc672-fall08/lectures/Lecture-10.pdf"


    // create item set 0, then itemset1 = goto(i0, x), ...
    /// Augment grammar
    augmentGrammar();

    /// calculate first set
    createFirstSet();

    if (debugprint) {
        printbuffer << "First Set:" << std::endl;
        for (std::string variable : grammar.getVariables()) {
            printbuffer << "  FIRST(" << variable << ") = { ";
            for (std::string terminal : first[variable]) {
                printbuffer << "\'" << terminal << "\' ";
            }
            printbuffer << "}" << std::endl;
        }
        printbuffer << "________________________________________" << std::endl;
    }
    /// compute item set 0: (item set 0 == closure(s))
    std::set<std::pair<Production, std::string>> itemSet0;
    for (Production prod : grammar.getProductions()) {
        if (prod.head == grammar.getStartState()) {
            // augment body (add '*' to indicate current parsing position)
            augmentRule(prod);
            itemSet0.insert(std::make_pair(prod, "EOS")); // lookahead of startstate is EOS (always)
        }
    }
    generateClosure(itemSet0);
    parserItemSets.insert(std::make_pair(0, itemSet0));

    /// compute GOTOs
    computeGOTO();

    if (debugprint) {
        printbuffer << "ALL ITEM SETS:" << std::endl;
        for (int i = 0; i<parserItemSets.size(); i++) {
            printbuffer << "Item Set " << i << " :" << std::endl;
            for (std::pair<Production, std::string> item: parserItemSets[i]) {
                printbuffer << "  [" << item.first.head << " -> ";
                for (std::string symbol: item.first.body) {
                    printbuffer << symbol;
                }
                if (item.second != "") {
                    printbuffer << ", " << item.second;
                } else {
                    printbuffer << ", NONE";
                }
                printbuffer << "]" << std::endl;
            }
            printbuffer << "________________________________________" << std::endl;
        }
    }

    /// fill GOTO table
    createGOTOTable();

    if (debugprint) {
        printbuffer << "GOTO TABLE:" << std::endl;
        for (auto tableEntry : gotoTable) {
            printbuffer << "  GOTO(" << tableEntry.first.first << ", " << tableEntry.first.second << ") = " << tableEntry.second << std::endl;
        }
        printbuffer << "________________________________________" << std::endl;
    }

    /// fill ACTION table
    createACTIONTable();

    if (debugprint) {
        printbuffer << "ACTION TABLE:" << std::endl;
        for (auto tableEntry : actionTable) {
            printbuffer << "  ACTION(" << tableEntry.first.first << ", " << tableEntry.first.second << ") = ";
            for (std::string b : tableEntry.second) {
                printbuffer << b;
            }
            printbuffer << std::endl;
        }
        printbuffer << "________________________________________" << std::endl;
    }
}

bool LR1Parser::augmentGrammar() {
    // add new starting state
    grammar.addVariable("S");
    grammar.addProduction(Production("S", {grammar.getStartState()}));
    grammar.setStartState("S");
}

///////////////////////////////////////////////////////////////////////
/*
 * PUBLIC FUNCTIONS
 */
const std::stringstream &LR1Parser::getPrintbuffer() const {
    return printbuffer;
}

bool LR1Parser::parse(std::vector<std::string> input) {
    if (debugprint) {
        printbuffer.str(std::string());
        printbuffer << "PARSER INPUT: ";
        for (auto s : input) {
            printbuffer << s;
        }
        printbuffer << std::endl;
    }
    /// parse table constructed after constructor is called
    /// parse:
    parserStack.push("0"); // state
    std::string inpSymbol = "";
    std::string action = "";
    while (action != "accept") {
        if (input.size() > 0) {
            inpSymbol = input[0];
        }
        else {
            inpSymbol = "EOS";
        }
        if (inpSymbol != "EOS" and std::find(grammar.getTerminals().begin(), grammar.getTerminals().end(), inpSymbol) == grammar.getTerminals().end()) {
            //std::cerr << "LR PARSER ERROR: parser input is not in the given grammar" << std::endl;
            return false;
        }
        std::vector<std::string> event = actionTable[{std::stoi(parserStack.top()), inpSymbol}];
        if (event.empty()) {
            //std::cerr << "LR PARSER ERROR: parser input is not valid (action is empty, state + input symbol is not valid)" << std::endl;
            return false;
        }
        action = event[0];

        if (debugprint) {
            for (std::string s : event) {
                if (s == "*") { continue; }
                if (s != event[0]) {
                    printbuffer << " ";
                }
                printbuffer << s;
            }
            printbuffer << std::endl;
        }

        if (action == "shift") {
            // push input onto stack, push new state onto stack
            parserStack.push(inpSymbol);
            parserStack.push(event[1]);
            // remove input symbol from input
            input.erase(input.begin());
        }
        else if (action == "reduce") {
            std::string reduceTO = event[1]; // production head
            std::vector<std::string> reduceFrom;
            for (int i = event.size()-1; i > 2; i--) { // the production body is put in reverse
                if (event[i] == "*") {continue;} // don't include "*" in production
                reduceFrom.emplace_back(event[i]);
            }
            // reduce the stack
            while (!reduceFrom.empty()) {
                parserStack.pop(); // remove state
                if (reduceFrom[0] != parserStack.top()) { /*std::cerr << "LR PARSER ERROR: reduce action error" << std::endl;*/ return false; }
                parserStack.pop(); // pop symbol
                reduceFrom.erase(reduceFrom.begin());
            }
            // get next state
            int nextState = gotoTable[{std::stoi(parserStack.top()), reduceTO}];
            parserStack.push(reduceTO);
            parserStack.push(std::to_string(nextState));
        }
    }
    return true; // action must be "accept" (it left the while loop)
}

void LR1Parser::saveParser(std::string fileName) {
    /*
     * this function is here to save time
     * it saves the constructed LR1 parser into file format
     * it is not needed for the functionality of the parser
     *
     * Saves:   parserItemSets
     *          gotoTable
     *          actionTable
     */

    /// parserItemSets
    nlohmann::json j;
    for (auto itemSet : parserItemSets) {
        nlohmann::json itemSetObject;
        itemSetObject["itemSetNumber"] = itemSet.first;
        for (auto item : itemSet.second) {
            nlohmann::json itemObject;
            itemObject["head"] = item.first.head;
            itemObject["body"] = item.first.body;
            itemObject["lookahead"] = item.second;
            itemSetObject["itemSet"].emplace_back(itemObject);
        }

        j["parserItemSets"].emplace_back(itemSetObject);
    }
    /// gotoTable
    for (auto tableEntry : gotoTable) {
        nlohmann::json tableEntryObject;
        nlohmann::json tableEntryKeyObject;
        tableEntryKeyObject["parentItemSetNumber"] = tableEntry.first.first;
        tableEntryKeyObject["recognizedSymbol"] = tableEntry.first.second;

        tableEntryObject["key"] = tableEntryKeyObject;
        tableEntryObject["gotoSetNumber"] = tableEntry.second;

        j["gotoTable"].emplace_back(tableEntryObject);
    }
    /// actionTable
    for (auto tableEntry : actionTable) {
        nlohmann::json tableEntryObject;
        nlohmann::json tableEntryKeyObject;
        tableEntryKeyObject["parentItemSetNumber"] = tableEntry.first.first;
        tableEntryKeyObject["recognizedSymbol"] = tableEntry.first.second;

        tableEntryObject["key"] = tableEntryKeyObject;
        tableEntryObject["action"] = tableEntry.second;

        j["actionTable"].emplace_back(tableEntryObject);
    }

    /// print to file
    std::string destination = "../res/";
    destination += fileName + ".json";
    std::ofstream outputFile(destination);
    outputFile << std::setw(4) << j;
}
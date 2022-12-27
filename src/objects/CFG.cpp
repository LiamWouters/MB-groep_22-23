#include "CFG.h"
#include "../../lib/nlohmann-json/json.hpp"
#include "../utilities/utilities.h"

#include <string>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

productions sortProductions(const productions& vec) {
    productions result = vec;
    std::sort(result.begin(), result.end());
    return result;
}

std::string printProductions(const productions& vec) {
    std::string result;
    std::vector<std::string> prodStrings;
    const std::string& whiteSpace = "    ";
    for (const auto& prod : vec) {
        prodStrings.emplace_back(prod.toString());
    }
    std::sort(prodStrings.begin(), prodStrings.end());
    result += "{\n";
    productions sortedVec = sortProductions(vec);
    for (const auto& x : prodStrings) {
        result += whiteSpace;
        result += x;
        result += "\n";
    }
    result += "}";
    return result;
}

CFG::CFG() {
    /*
    v.emplace_back("BINDIGIT");
    v.emplace_back("S");

    t.emplace_back("0");
    t.emplace_back("1");
    t.emplace_back("a");
    t.emplace_back("b");

    p["BINDIGIT"].emplace_back("0");
    p["BINDIGIT"].emplace_back("1");
    p["S"].emplace_back("");
    p["S"].emplace_back("a S b BINDIGIT");

    s = "S";
     */
}

void CFG::print(std::ostream& out) const {
    std::string toPrint;
    toPrint += "V = " + printBrackets(v) + '\n';
    toPrint += "T = " + printBrackets(t) + '\n';
    toPrint += "P = " + printProductions(p) + '\n';
    toPrint += "S = " + s;
    out << toPrint << std::endl;
}

CFG::CFG(const std::string path) {
    // load json
    std::ifstream input(path);
    json j;
    input >> j;
    v = j["Variables"].get<std::vector<std::string>>();
    t = j["Terminals"].get<std::vector<std::string>>();
    s = j["Start"].get<std::string>();
    for (int i = 0; i < j["Productions"].size(); i++) {
        Production prod(j["Productions"][i]["head"], j["Productions"][i]["body"].get<std::vector<std::string>>());
        p.emplace_back(prod);
    }
}

CFG::CFG(const CFG &that) : v(that.v), t(that.t), p(that.p), s(that.s) {}

/*
 * Getters/ Setters [USE WITH CAUTION]
 */
productions CFG::getProductions() {return p;}
std::string CFG::getStartState() {return s;}
std::vector<std::string> CFG::getTerminals() {return t;}
std::vector<std::string> CFG::getVariables() {return v;}

void CFG::setVariables(std::vector<std::string> var) {v = var;}
void CFG::setTerminals(std::vector<std::string> ter) {t = ter;}
void CFG::setProductions(productions prod) {p = prod;}
void CFG::setStartState(std::string start) {s = start;}

void CFG::addVariable(std::string var) {v.emplace_back(var);}
void CFG::addTerminal(std::string ter) {t.emplace_back(ter);}
void CFG::addProduction(Production prod) {p.emplace_back(prod);}

int CFG::countProductions() const { return p.size(); }

int CFG::countVariables() const { return v.size(); }

int CFG::countTerminals() const { return t.size(); }

std::set<std::string> CFG::findNullableVariables() const {
    std::set<std::string> nullableV;
    for (auto& x :
         p) { // BASIS, find nullable variables and store in nullableV, temp1 is p with A->"" productions removed
        if (x.body.empty()) {
            nullableV.emplace(x.head);
        }
    }
    bool found = true; // INDUCTION, find nullable variables and store in nullableV
    while (found) {
        found = false;
        for (auto& x : p) {
            if (nullableV.find(x.head) != nullableV.end()) { // skip production if variable already found to be nullable
                continue;
            }
            if (vectorContainsOnly(x.body, nullableV)) {
                nullableV.emplace(x.head);
                found = true;
            }
        }
    } // all nullable variables found and stored in nullableV, temp 1 contains all non A -> "" productions
    return nullableV;
}

void CFG::replaceProductionRuleEps(const std::set<std::string>& nullableV, productions& temp, const Production& x,
                                   int m) const {
    unsigned int powSetSize = pow(2, m); // calculate amount of new production rules
    int counter, j;
    for (counter = 0; counter < powSetSize; counter++) { // counter 0000...0_m to 1111...1_m
        std::vector<std::string> prodBody;
        int nonNullableVariablesEncountered = 0;
        for (j = 0; j < x.body.size(); j++) {         // loop over production body
            if (!stringInSet(x.body[j], nullableV)) { // if symbol is not nullable
                ++nonNullableVariablesEncountered;
                prodBody.emplace_back(x.body[j]);
            } else if (counter & (1 << (j - nonNullableVariablesEncountered))) { // if symbol is nullable
                prodBody.emplace_back(x.body[j]);                                // check if bit j-nonNull... = 1
            }
        }
        Production prod(x.head, prodBody); // make production
        if (!prodBody.empty()) {
            temp.emplace_back(prod); // exclude case all nullable variables not present
        }
    }
}

void CFG::constructCFGWithoutNullables(const std::set<std::string>& nullableV) {
    productions temp;
    for (const auto& x : p) { // add new production rules
        int m = 0;
        for (auto& i : x.body) {
            if (stringInSet(i, nullableV)) { // count nullable variables in production body
                ++m;
            }
        }
        if (x.body.empty()) {
            continue;
        }
        if (m == 0) {
            temp.emplace_back(x);
            continue;
        }
        replaceProductionRuleEps(nullableV, temp, x, m);
    }
    this->p = temp;
}

void CFG::eliminateEpsProductionsP(std::ostream& out) {
    int originalProductionCount = countProductions();
    std::set<std::string> nullableV = findNullableVariables();
    constructCFGWithoutNullables(nullableV);
    out << ">> Eliminating epsilon productions" << std::endl;
    out << "Nullables are " << printSetDelimited(nullableV, ", ") << std::endl;
    out << "Created " << countProductions() << " productions, original had " << originalProductionCount << std::endl
        << std::endl;
}

bool CFG::isUnitProduction(const Production& prod) const {
    if (prod.body.size() == 1) {
        if (stringInVector(prod.body[0], v)) {
            return true;
        }
    }
    return false;
}

void CFG::findUnitPairsBasis(std::set<unitPair>& unitPairs) const {
    for (const auto& x : v) {
        if (stringInVector(x, v)) {
            unitPairs.emplace(std::make_pair(x, x));
        }
    }
}

void CFG::findUnitPairsInduction(std::set<unitPair>& unitPairs) const {
    bool found = true;
    while (found) {
        int prevSize = unitPairs.size();
        found = false;
        for (const auto& prod : p) {
            if (!isUnitProduction(prod)) {
                continue;
            }
            for (const auto& unitP : unitPairs) {
                if (prod.head == unitP.second) {
                    unitPairs.emplace(std::make_pair(unitP.first, prod.body[0]));
                }
            }
        }
        if (prevSize != unitPairs.size()) {
            found = true;
        }
    }
}

void CFG::constructCFGWithoutUnitPairs(const std::set<unitPair>& unitPairs) {
    productions temp;
    for (const auto& unitP : unitPairs) {
        for (const auto& prod : p) {
            if (prod.head != unitP.second) {
                continue;
            }
            if (isUnitProduction(prod)) {
                continue;
            }
            Production product(unitP.first, prod.body);
            temp.emplace_back(product); // POSSIBLE DUPLICATES?? solution work with set
        }
    }
    this->p = temp;
}

void CFG::eliminateUnitPairsP(std::ostream& out) {
    int UnitProductionCount = countUnitProductions();
    int originalProductionCount = countProductions();
    std::set<unitPair> unitPairs;
    findUnitPairsBasis(unitPairs);
    findUnitPairsInduction(unitPairs);
    constructCFGWithoutUnitPairs(unitPairs);

    out << ">> Eliminating unit pairs" << std::endl;
    out << "Found " << UnitProductionCount << " unit productions" << std::endl;
    out << "Unit pairs: " << printSetOfPairsDelimited(unitPairs, ", ") << std::endl;
    out << "Created " << countProductions() << " new productions, original had " << originalProductionCount << std::endl
        << std::endl;
}

int CFG::countUnitProductions() const {
    int count = 0;
    for (auto& x : p) {
        if (isUnitProduction(x)) {
            count++;
        }
    }
    return count;
}

bool CFG::isTerminal(const std::string& symbol) const { return stringInVector(symbol, t); }

bool CFG::isVariable(const std::string& symbol) const { return stringInVector(symbol, v); }

std::set<std::string> CFG::findGeneratingSymbols() const {
    std::set<std::string> result;
    for (const auto& terminal : t) {
        result.emplace(terminal);
    }
    bool found = true;
    while (found) {
        int prevSize = result.size();
        found = false;
        for (const auto& prod : p) {
            if (prod.body.empty()) {
                result.emplace(prod.head);
                continue;
            }
            if (vectorContainsOnly(prod.body, result)) {
                result.emplace(prod.head);
            }
        }
        if (prevSize != result.size()) {
            found = true;
        }
    }
    return result;
}

std::set<std::string> CFG::findReachableSymbols() const {
    std::set<std::string> result;
    result.emplace(s);
    bool found = true;
    while (found) {
        int prevSize = result.size();
        found = false;
        for (const auto& prod : p) {
            if (!stringInSet(prod.head, result)) {
                continue;
            }
            for (const auto& symbol : prod.body) {
                result.emplace(symbol);
            }
        }
        if (prevSize != result.size()) {
            found = true;
        }
    }
    return result;
}

std::set<std::string> CFG::findUsefulSymbols(const std::set<std::string>& gen,
                                             const std::set<std::string>& reach) const {
    return setIntersection(gen, reach);
}

void CFG::removeSymbols(const std::set<std::string>& symbols) {
    std::vector<std::string> tempVar;
    std::vector<std::string> tempTerm;
    productions tempProd;
    for (const auto& var : v) {
        if (stringInSet(var, symbols)) {
            tempVar.emplace_back(var);
        }
    }
    for (const auto& term : t) {
        if (stringInSet(term, symbols)) {
            tempTerm.emplace_back(term);
        }
    }
    for (const auto& prod : p) {
        if (!stringInSet(prod.head, symbols)) {
            continue;
        }
        if (vectorContainsOnly(prod.body, symbols)) {
            tempProd.emplace_back(prod);
        }
    }
    this->v = tempVar;
    this->t = tempTerm;
    this->p = tempProd;
}

void CFG::eliminateUselessSymbolsP(std::ostream& out) {
    int originalVariableCount = countVariables();
    int originalTerminalCount = countTerminals();
    int originalProductionCount = countProductions();

    std::set<std::string> generating = findGeneratingSymbols();
    removeSymbols(generating);
    std::set<std::string> reachable = findReachableSymbols();
    removeSymbols(reachable);
    std::set<std::string> useful = findUsefulSymbols(generating, reachable);

    int removedVariableCount = originalVariableCount - countVariables();
    int removedTerminalCount = originalTerminalCount - countTerminals();
    int removedProductionCount = originalProductionCount - countProductions();

    out << ">> Eliminating useless symbols" << std::endl;
    out << "Generating symbols: " << printSetDelimited(generating, ", ") << std::endl;
    out << "Reachable symbols: " << printSetDelimited(reachable, ", ") << std::endl;
    out << "Useful symbols: " << printSetDelimited(useful, ", ") << std::endl;
    out << "Removed " << removedVariableCount << " variables, ";
    out << removedTerminalCount << " terminals ";
    out << "and " << removedProductionCount << " productions" << std::endl << std::endl;
}

int CFG::countReplacements(const std::string& h) const {
    int count = 0;
    for (const auto& prod : p) {
        if (prod.head == h) {
            ++count;
        }
    }
    return count;
}

void CFG::makeSimpleReplacements() {
    std::vector<std::pair<std::string, std::string>> replP;
    for (const auto& prod : p) {
        if (prod.body.size() == 1 and countReplacements(prod.head) == 1) {
            replP.emplace_back(std::make_pair(prod.body[0], prod.head));
        }
    }
    for (const auto& x : replP) {
        for (auto& prod : p) {
            if (prod.body.size() == 1 and prod.head == x.second) {
                continue;
            }
            for (auto& symbol : prod.body) {
                if (x.first == symbol) {
                    symbol = x.second;
                }
            }
        }
    }
}

void CFG::findBadTerminals(std::set<std::string>& badTerminals) const {
    // possible CHANGE only add to badTerminalList if no A -> a present and no other production A -> w present
    for (auto& prod : p) {
        if (prod.body.size() < 2) {
            continue;
        }
        for (auto& symbol : prod.body) { //  if production has more than 2 symbols, loop productions
            if (isTerminal(symbol)) {    // if symbol is terminal
                badTerminals.emplace(symbol);
            }
        }
    }
}

void CFG::replaceBadTerminals(const std::set<std::string>& badTerminals) {
    for (auto& prod : p) {
        for (auto& symbol : prod.body) {
            if (stringInSet(symbol, badTerminals)) {
                symbol.insert(0, "_");
            }
        }
    }
}

void CFG::addVariablesAndProductions(std::vector<std::string>& newVars, const std::set<std::string>& badTerminals) {
    for (const auto& term : badTerminals) {
        v.emplace_back("_" + term);
        newVars.emplace_back("_" + term);
        p.emplace_back(Production("_" + term, {term}));
    }
}

void CFG::replaceTerminalsBadBodiesP(std::ostream& out) {
    int originalVariableCount = countVariables();
    int originalProductionCount = countProductions();

    std::set<std::string> badTerms;
    std::vector<std::string> newVars;
    makeSimpleReplacements();
    findBadTerminals(badTerms);
    replaceBadTerminals(badTerms);
    addVariablesAndProductions(newVars, badTerms);

    int addedVariableCount = countVariables() - originalVariableCount;

    out << ">> Replacing terminals in bad bodies" << std::endl;
    out << "Added " << addedVariableCount << " new variables: " << printBrackets(newVars) << std::endl;
    out << "Created " << countProductions() << " new productions, original had " << originalProductionCount << std::endl
        << std::endl;
}

int CFG::breakBodies() {
    int nrBroken = 0;
    productions temp;
    std::map<std::string, int> mapCounters;
    for (const auto& var : v) {
        mapCounters[var] = 0;
    }
    for (const auto& prod : p) {
        int k = prod.body.size();
        if (k < 3) {
            temp.emplace_back(prod);
            continue;
        }
        ++nrBroken;
        int offset = mapCounters[prod.head];
        for (int i = 0; i <= k - 2; i++) {
            std::string h;
            std::string var1;
            std::string var2;
            if (i == 0) {
                h = prod.head;
                var1 = prod.body[0];
                var2 = prod.head + "_" + std::to_string(2 + offset);
            } else if (i == k - 2) {
                h = prod.head + "_" + std::to_string(k - 1 + offset); // C2
                var1 = prod.body[k - 2];
                var2 = prod.body[k - 1];
                v.emplace_back(h);
                ++mapCounters[prod.head];
            } else {
                h = prod.head + "_" + std::to_string(i + offset + 1); // C2
                var1 = prod.body[i];
                var2 = prod.head + "_" + std::to_string(i + offset + 2);
                v.emplace_back(h);
                ++mapCounters[prod.head];
            }
            temp.emplace_back(Production(h, {var1, var2}));
        }
    }
    this->p = temp;
    return nrBroken;
}

void CFG::breakBadBodiesP(std::ostream& out) {
    int originalVariableCount = countVariables();
    int nrBroken = breakBodies();
    int addedVariableCount = countVariables() - originalVariableCount;

    out << ">> Broke " << nrBroken << " bodies, added " << addedVariableCount << " new variables" << std::endl;
    out << ">>> Result CFG:" << std::endl << std::endl;
}

void CFG::toCNF(std::ostream& out) {
    out << "Original CFG:" << std::endl << std::endl;
    print(out);
    out << "\n"
        << "-------------------------------------"
        << "\n"
        << std::endl;
    eliminateEpsProductionsP(out);
    print(out);
    out << "\n";
    eliminateUnitPairsP(out);
    print(out);
    out << "\n";
    eliminateUselessSymbolsP(out);
    print(out);
    out << "\n";
    replaceTerminalsBadBodiesP(out);
    print(out);
    out << "\n";
    breakBadBodiesP(out);
    print(out);
}

std::map<int, std::map<int, std::set<std::string>>> CFG::getCykTable(const std::vector<std::string>& w) const {
    std::map<int, std::map<int, std::set<std::string>>> table;
    int n = (int)w.size();
    for (int i = 0; i < n; i++) {
        for (auto& prod : p) {
            if (prod.body.size() == 1 and prod.body[0] == w[i]) { // terminal
                table[i][i].insert(prod.head);
            }
        }
        for (int l = i; l >= 0; l--) {
            for (int k = l; k <= i; k++) {
                for (auto prod : p) {
                    if (prod.body.size() == 2 and stringInSet(prod.body[0], table[l][k]) and
                        stringInSet(prod.body[1], table[k + 1][i])) {
                        table[l][i].insert(prod.head);
                    }
                }
            }
        }
    }
    return table;
}

void CFG::printCykTable(std::map<int, std::map<int, std::set<std::string>>>& table, int n, std::ostream& out) const {
    int c = 1; // counts number of sets on line
    for (int j = n - 1; j >= 0; j--) {
        for (int i = 0; i < c; i++) {
            if (i == 0) {
                out << "|";
            }
            out << " " << printSetDelimited(table[i][j + i], ", ") << "  |";
        }
        out << std::endl;
        c++;
    }
}

bool CFG::evaluateCykTable(std::map<int, std::map<int, std::set<std::string>>>& table, int n) const {
    return stringInSet(s, table[0][n - 1]);
}

bool CFG::accepts(const std::string& w, std::ostream& out) const {
    std::vector<std::string> vecW = stringToVector(w);
    int n = vecW.size();
    std::map<int, std::map<int, std::set<std::string>>> table = getCykTable(vecW);
    printCykTable(table, n, out);

    bool accepted = evaluateCykTable(table, n);
    if (accepted) {
        out << "true" << std::endl;
    } else {
        out << "false" << std::endl;
    }
    return accepted;
}

std::map<std::pair<std::string, std::string>, std::vector<std::string>> CFG::lltable() const {
    /*
     * This function determines the first and follow sets to be used by an LL parser.
     *
     * Rules for first sets:
     * 1. If X is a terminal then First(X) is just X!
     * 2. If there is a Production X → ε then add <EOS> to first(X)
     * 3. If there is a Production X → Y1Y2…Yk then add first(Y1Y2..Yk) to first(X), First(Y1Y2..Yk) is either:
     *      -First(Y1) (if First(Y1) doesn't contain ε)
     *      OR (if First(Y1) does contain ε) then First (Y1Y2..Yk) is First(Y1) \ ε + First(Y2..Yk)
     *      -If First(Y1) First(Y2)..First(Yk) all contain ε then add ε to First(Y1Y2…Yk) as well
     *
     * Rules for the follow sets:
     * 1. The starting symbol always has <EOS> in its follow set
     * 2. If there is a production A → aBb, then everything in FIRST(b) \ ε goes in FOLLOW(B)
     * 3. If there is a production A → aB, then everything in FOLLOW(A) is in FOLLOW(B)
     * 4. If there is a production A → aBb, where FIRST(b) contains ε, then everything in FOLLOW(A) is in FOLLOW(B)
     *
     * Final rules for constructing the parse table:
     * Step 3: For each production A –> α. (A tends to alpha):
     *  1. For each terminal in First(α), make entry A –> α in the table.
     *  2. If First(α) contains ε, then for each terminal in Follow(A), make entry A –>  ε in the table.
     *  3. If the First(α) contains ε and Follow(A) contains $, then make entry A –>  ε in the table for the $.
     *
     * Credits: https://stackoverflow.com/questions/3818330/what-are-first-and-follow-sets-used-for-in-parsing
     * And: https://www.geeksforgeeks.org/construction-of-ll1-parsing-table/
     */
    // Some containers to store information.
    std::map<std::string, std::vector<std::string>> first;
    std::map<std::string, std::vector<std::string>> follow;
    std::map<std::string, std::vector<std::string>> check;
    std::map<std::string, std::vector<std::string>> followFollow;
    std::map<std::pair<std::string, std::string>, std::vector<std::string>> expect;
    std::vector<std::string> tableElements = t;
    tableElements.emplace_back("<EOS>");
    std::set<std::string> nullables = findNullableVariables();
    // Initialize maps with all empty vectors
    for(auto &i: v){
        first[i] = {}; follow[i] = {}; followFollow[i] = {}; expect[std::make_pair(i, "<EOS>")] = {"<ERR>"};
        for(auto &j: t){expect[std::make_pair(i, j)] = {"<ERR>"};}
    }
    // Determine the easy and hard sets
    for(auto &i: p){
        std::string var = i.head;
        if(i.body.empty()){
            first[var].emplace_back(""); continue;
        }
        std::string firstchar = i.body[0];
        if(isTerminal(firstchar)){
            first[var].emplace_back(firstchar); continue;
        }
        else{
            int j = 0;
            std::vector<std::string> checkOrder = {firstchar};
            if(i.body.size() == 1){
                if(nullables.find(firstchar) != nullables.end()){first[i.head].emplace_back("<EOS>");}
                check[i.head].emplace_back(i.body[0]); continue;
            }
            while(nullables.find(firstchar) != nullables.end()){
                j += 1; firstchar = i.body[j]; checkOrder.emplace_back(i.body[j]);
            }
            if(j == i.body.size()-1){first[var].emplace_back("<EOS>");}
            else{
                if(isTerminal(i.body[j+1])){first[var].emplace_back(i.body[j+1]);}
                else{checkOrder.emplace_back(i.body[j+1]);}
            }
            for(auto &k:checkOrder){check[i.head].emplace_back(k);}
        }
    }
    // Determine the "harder" first sets.
    bool noChecks = check.empty();
    while(!noChecks){
        auto i = check.begin();
        while(i != check.end()){
            auto j = i->second.begin();
            while(j != i->second.end()){
                bool noCheckVars = true;
                if(!check[*j].empty()){noCheckVars = false;}
                if(i->first == *j){i->second.erase(j); continue;}
                if(noCheckVars){
                    for(auto &k: first[*j]){
                        if(std::find(first[i->first].begin(), first[i->first].end(), k) == first[i->first].end()){
                            first[i->first].emplace_back(k);
                        }
                    }
                    i->second.erase(j);
                }
                else{j++;}
            }
            if(i->second.empty()){i = check.erase(i);}
            else{i++;}
        } noChecks = true;
        for(auto &k: check){
            if(!k.second.empty()){noChecks = false; break;}
        }
    }
    // First sets are now properly determined, now onto the follow sets
    follow[s].emplace_back("<EOS>");
    for(auto &pr: p){
        for(int i = 0; i < pr.body.size(); i++){
            if(isVariable(pr.body[i])){
                if(i == pr.body.size()-1){
                    if(isVariable(pr.body[i]) && std::find(followFollow[pr.body[i]].begin(), followFollow[pr.body[i]].end(), pr.head) == followFollow[pr.body[i]].end()){
                        followFollow[pr.body[i]].emplace_back(pr.head);
                    }
                    if(std::find(follow[pr.body[i]].begin(), follow[pr.body[i]].end(), "<EOS>") == follow[pr.body[i]].end()){
                        follow[pr.body[i]].emplace_back("<EOS>");
                    } continue;
                }
                else if(isTerminal(pr.body[i+1])){
                    if(std::find(follow[pr.body[i]].begin(), follow[pr.body[i]].end(), pr.body[i+1]) == follow[pr.body[i]].end()){
                        follow[pr.body[i]].emplace_back(pr.body[i+1]);
                    }
                }
                else if(isVariable(pr.body[i+1])){
                    for(auto &j: first[pr.body[i+1]]){
                        if(std::find(follow[pr.body[i]].begin(), follow[pr.body[i]].end(), j) == follow[pr.body[i]].end()){
                            follow[pr.body[i]].emplace_back(j);
                        }
                    }
                    bool null = true;
                    int j = i+1;
                    while(j < pr.body.size()-1){
                        if(isTerminal(pr.body[j])){null = false; break;}
                        else if(nullables.find(pr.body[j]) == nullables.end()){null = false; break;}
                    }
                    if(null && std::find(followFollow[pr.body[i]].begin(), followFollow[pr.body[i]].end(), pr.head) == followFollow[pr.body[i]].end()) {
                        followFollow[pr.body[i]].emplace_back(pr.head);
                    }
                }
            }
        }
    }
    // Remove the statements that imply that follow(A) must be added to itself or empty vectors
    auto k = followFollow.begin();
    std::vector<std::string> incompleteVars;
    while(k != followFollow.end()){
        if(k->second.empty()){k = followFollow.erase(k);}
        else{
            auto l = k->second.begin();
            while(l != k->second.end()){
                if(*l == k->first){k->second.erase(l);}
                else{l++;}
            }
            if(k->second.empty()){k = followFollow.erase(k);}
            else{
                incompleteVars.emplace_back(k->first); k++;
            }
        }
    }
    bool complete = followFollow.empty();
    while(!complete){
        auto m = followFollow.begin();
        while(m != followFollow.end()){
            auto n = m->second.begin();
            while(n != m->second.end()){
                std::string current = *n;
                if(std::find(incompleteVars.begin(), incompleteVars.end(), current) == incompleteVars.end()){
                    std::vector<std::string> combine;
                    std::set_union(follow[current].begin(), follow[current].end(), follow[m->first].begin(), follow[m->first].end(), std::back_inserter(combine));
                    for(auto &q: combine){
                        if(std::find(follow[m->first].begin(), follow[m->first].end(), q) == follow[m->first].end()){
                            follow[m->first].emplace_back(q);
                        }
                    }
                    m->second.erase(n);
                }
                else{n++;}
            }
            if(m->second.empty()){m = followFollow.erase(m);}
            else{m++;}
        }
        if(followFollow.empty()){complete = true;}
    }
    // First and follow sets are complete, now determine expected rules.
    for(auto &production: p){
        if(production.body.empty()){
            for(auto &i: follow[production.head]){
                expect[std::make_pair(production.head, i)] = {};
            } continue;
        }
        std::string firstChar = production.body[0];
        if(isTerminal(firstChar)){expect[std::make_pair(production.head, firstChar)] = production.body;}
        else{
            for(auto &l: first[firstChar]){
                if(l.empty()){continue;}
                expect[std::make_pair(production.head, l)] = production.body;
            }
            if(std::find(first[firstChar].begin(), first[firstChar].end(), "") != first[firstChar].end()){
                for(auto &m: follow[firstChar]){
                    expect[std::make_pair(production.head, m)] = {};
                }
                if(std::find(follow[firstChar].begin(), follow[firstChar].end(), "<EOS>") != follow[firstChar].end()){
                    expect[std::make_pair(production.head, "<EOS>")] = {};
                }
            }
        }
    }
    return expect;
}
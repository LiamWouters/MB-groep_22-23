#include "CFG.h"
#include "../../lib/nlohmann-json/json.hpp"
#include "../utilities/utilities.h"

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

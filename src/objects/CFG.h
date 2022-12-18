#ifndef CFG_CFG_H
#define CFG_CFG_H

#include "Production.h"
#include <iostream>
#include <map>
#include <set>

using productions = std::vector<Production>;
using unitPair = std::pair<std::string, std::string>;

class CFG {
public:
    /*
     * Constructors
     */
    CFG();
    CFG(std::string path);
    CFG(const CFG &that);

    /*
     * Getters/ Setters [USE WITH CAUTION]
     */
    std::vector<std::string> getVariables();
    std::vector<std::string> getTerminals();
    productions getProductions();
    std::string getStartState();

    void setVariables(std::vector<std::string> var);
    void setTerminals(std::vector<std::string> ter);
    void setProductions(productions prod);
    void setStartState(std::string start);

    void addVariable(std::string var);
    void addTerminal(std::string ter);
    void addProduction(Production prod);

    void print(std::ostream& out = std::cout) const;

    void eliminateEpsProductionsP(std::ostream& out);

    void eliminateUnitPairsP(std::ostream& out);

    void eliminateUselessSymbolsP(std::ostream& out);

    void replaceTerminalsBadBodiesP(std::ostream& out);

    void toCNF(std::ostream& out = std::cout);

    void breakBadBodiesP(std::ostream& out);

    bool accepts(const std::string& w, std::ostream& out = std::cout) const;

    std::map<std::pair<std::string, std::string>, std::vector<std::string>> lltable() const;

private:
    int countProductions() const;

    int countVariables() const;

    int countTerminals() const;

    std::set<std::string> findNullableVariables() const;

    bool isUnitProduction(const Production& prod) const;

    int countUnitProductions() const;

    void findUnitPairsBasis(std::set<unitPair>& unitPairs) const;

    void findUnitPairsInduction(std::set<unitPair>& unitPairs) const;

    void constructCFGWithoutUnitPairs(const std::set<unitPair>& unitPairs);

    void constructCFGWithoutNullables(const std::set<std::string>& nullableV);

    void replaceProductionRuleEps(const std::set<std::string>& nullableV, productions& temp, const Production& x,
                                  int m) const;

    bool isTerminal(const std::string& symbol) const;

    bool isVariable(const std::string& symbol) const;

    std::set<std::string> findGeneratingSymbols() const;

    std::set<std::string> findReachableSymbols() const;

    std::set<std::string> findUsefulSymbols(const std::set<std::string>& gen, const std::set<std::string>& reach) const;

    void removeSymbols(const std::set<std::string>& symbols);

    int countReplacements(const std::string& h) const;

    void makeSimpleReplacements();

    void findBadTerminals(std::set<std::string>& badTerminals) const;

    void replaceBadTerminals(const std::set<std::string>& badTerminals);

    void addVariablesAndProductions(std::vector<std::string>& newVars, const std::set<std::string>& badTerminals);

    int breakBodies();

    std::map<int, std::map<int, std::set<std::string>>> getCykTable(const std::vector<std::string>& w) const;

    void printCykTable(std::map<int, std::map<int, std::set<std::string>>>& table, int n, std::ostream& out) const;

    bool evaluateCykTable(std::map<int, std::map<int, std::set<std::string>>>& table, int n) const;

    std::vector<std::string> v;
    std::vector<std::string> t;
    productions p;
    std::string s;
};

#endif // CFG_CFG_H

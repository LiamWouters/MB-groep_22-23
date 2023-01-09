//
// Created by axeld on 18/12/2022.
//

#ifndef MB_GROEP_22_23_LL1PARSER_H
#define MB_GROEP_22_23_LL1PARSER_H

#include <string>
#include <map>
#include <vector>
#include <bits/stdc++.h>
#include "CFG.h"
#include "Token.h"

class LL1Parser {
public:
    explicit LL1Parser(const CFG&);
    bool accepts(const std::vector<token>&);
private:
    CFG grammar;
    std::map<std::pair<std::string, std::string>, std::vector<std::vector<std::string>>> parseTable;
    static int match(std::vector<token>&, std::vector<token>&);
    static void pop_front(std::vector<token>&);
    static std::pair<unsigned int, unsigned int> mostProgress(std::map<int, int>&);
    static void printErrorReport(const std::pair<std::vector<token>, std::vector<token>>&, const std::string&);
};

#endif //MB_GROEP_22_23_LL1PARSER_H

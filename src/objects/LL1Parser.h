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
    std::pair<bool, int> accepts(const std::vector<token>&);
private:
    CFG grammar;
    std::map<std::pair<std::string, std::string>, std::vector<std::vector<std::string>>> parseTable;
    std::map<std::string, std::vector<std::string>> firstTable;
    static int match(std::vector<token>&, std::vector<token>&);
    static void pop_front(std::vector<token>&);
    static std::vector<unsigned int> mostProgress(std::map<int, int>&, std::vector<std::vector<token>>&);
    int printErrorReport(const std::pair<std::vector<std::vector<token>>, std::vector<std::vector<token>>>&, const std::vector<std::string>&, std::vector<unsigned int>&, const std::vector<token>&);
};

#endif //MB_GROEP_22_23_LL1PARSER_H

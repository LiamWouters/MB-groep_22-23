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

class LL1Parser {
public:
    explicit LL1Parser(const CFG&);
    bool accepts(const std::vector<std::string>&);
private:
    CFG grammar;
    static bool match(std::vector<std::string>&, std::vector<std::string>&);
};

#endif //MB_GROEP_22_23_LL1PARSER_H

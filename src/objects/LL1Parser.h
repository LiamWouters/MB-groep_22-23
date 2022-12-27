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
    static bool match(std::vector<token>&, std::vector<token>&);
};

#endif //MB_GROEP_22_23_LL1PARSER_H

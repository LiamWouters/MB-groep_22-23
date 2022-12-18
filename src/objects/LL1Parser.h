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
    bool accepts(const std::string&);
private:
    CFG grammar;
    bool match(std::string&, std::string&);
};

#endif //MB_GROEP_22_23_LL1PARSER_H

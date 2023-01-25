//
// Created by axeld on 15/12/2022.
//

#ifndef MB_GROEP_22_23_SYNTAXHIGHLIGHTER_H
#define MB_GROEP_22_23_SYNTAXHIGHLIGHTER_H

#include <string>
#include <fstream>
#include <iostream>
#include "Token.h"


class SyntaxHighlighter {
public:
    // These use a path to a file (= old).
    static void jsonToHTML(const std::string & path);
    static void customToHTML(const std::string & path);
    // These use a vector of tokens and an error index.
    static void jsonToHTML2(std::vector<token>&, int&);
    static void customToHTML2(std::vector<token>&, int&);
};

#endif //MB_GROEP_22_23_SYNTAXHIGHLIGHTER_H

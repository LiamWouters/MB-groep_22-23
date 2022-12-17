//
// Created by axeld on 15/12/2022.
//

#ifndef MB_GROEP_22_23_SYNTAXHIGHLIGHTER_H
#define MB_GROEP_22_23_SYNTAXHIGHLIGHTER_H

#include <string>
#include <fstream>
#include <iostream>

class SyntaxHighlighter {
public:
    static void jsonToHTML(const std::string & path);
    static void customToHTML(const std::string & path);
};

#endif //MB_GROEP_22_23_SYNTAXHIGHLIGHTER_H

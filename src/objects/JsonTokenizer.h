#ifndef MB_GROEP_22_23_JSONTOKENIZER_H
#define MB_GROEP_22_23_JSONTOKENIZER_H

#include "Token.h"
#include <vector>

class JsonTokenizer {
public:
    // divides file found in path into tokens and stores them into member 'tokens'
    void tokenize(const std::string& path);

    void tokenizeString(const std::string& input);

    // divides file found in path into strings and stores them into member 'strings'
    void split(const std::string& path);

    // print member 'tokens' to out
    void printTokens(std::ostream& out) const;

    // print member 'strings' to out
    void printStrings(std::ostream& out) const;

    std::vector<token> tokens;
    std::vector<std::string> strings;
};

#endif // MB_GROEP_22_23_JSONTOKENIZER_H

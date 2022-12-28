#ifndef MB_GROEP_22_23_JSONTOKENIZER_H
#define MB_GROEP_22_23_JSONTOKENIZER_H

#include "Token.h"
#include <iostream>
#include <vector>

class JsonTokenizer {
public:
    // divides file found in path into tokens and stores them into member 'tokens'
    void tokenize(const std::string& path);

    void tokenizeString(const std::string& input);

    // divides file found in path into strings and stores them into member 'strings'
    void split(const std::string& path);

    // print member 'tokens' to out
    void printTokens(std::ostream& out = std::cout) const;

    // print member 'strings' to out
    void printStrings(std::ostream& out = std::cout) const;

    void tokenizeSimplified(const std::string& path);

    void reset();

    // bool isDouble(const std::string& s) const;
    bool isJsonNumber(const std::string& s) const;

    std::vector<token> tokens;
    std::vector<std::string> strings;

private:
    void increaseRow(unsigned int& l, unsigned int& c, char ch) const;
    void fixNumberTokens();
};

#endif // MB_GROEP_22_23_JSONTOKENIZER_H

#ifndef MB_GROEP_22_23_JSONTOKENIZER_H
#define MB_GROEP_22_23_JSONTOKENIZER_H

#include "Token.h"
#include <vector>

class JsonTokenizer {
public:
    void tokenize(const std::string& path);
    std::vector<token> tokens;
    std::vector<std::string> strings;
};

#endif // MB_GROEP_22_23_JSONTOKENIZER_H

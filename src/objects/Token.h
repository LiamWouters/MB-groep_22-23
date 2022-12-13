#ifndef MB_GROEP_22_23_TOKEN_H
#define MB_GROEP_22_23_TOKEN_H

#include <string>

struct position {
    position(unsigned int l, unsigned int c);
    unsigned int line;
    unsigned int column;
};

struct token {
    token(std::string cont, unsigned int l, unsigned int c);
    token(std::string cont, const position& p);
    std::string content;
    position pos;
};

#endif // MB_GROEP_22_23_TOKEN_H

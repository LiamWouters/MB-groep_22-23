#ifndef MB_GROEP_22_23_TOKEN_H
#define MB_GROEP_22_23_TOKEN_H

#include <string>

struct position {
    // constructor
    position(unsigned int l, unsigned int c);

    // variables
    unsigned int line;
    unsigned int column;
};

struct token {
    // constructors
    token(std::string cont, unsigned int l, unsigned int c);
    token(std::string cont, const position& p);

    // other functions
    void print(std::ostream& out) const;

    // variables
    std::string content;
    position pos;
};

#endif // MB_GROEP_22_23_TOKEN_H

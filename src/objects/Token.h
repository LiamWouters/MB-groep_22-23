#ifndef MB_GROEP_22_23_TOKEN_H
#define MB_GROEP_22_23_TOKEN_H

#include <iostream>
#include <string>

/*
enum class Type{
    CURLY_OPEN,
    CURLY_CLOSE,
    COLON,
    STRING,
    NUMBER,
    ARRAY_OPEN,
    ARRAY_CLOSE,
    COMMA,
    BOOLEAN,
    NULL_TYPE,
    INVALID_STRING_MISSING_QUOTE,
    INVALID_STRING_INVALID_CHARACTER,
    INVALID_NUMBER,
    UNKNOWN
};
*/

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
    token(std::string cont, const position& p, std::string t);
    token(std::string cont, unsigned int l, unsigned int c,  std::string t);

    // other functions
    void print(std::ostream& out = std::cout) const;
    void reset();

    // variables
    bool addedToStructure; // variable for creating data structure for file conversion (to avoid adding the token multiple time)
    std::string content;
    position pos;
    std::string type;
};

#endif // MB_GROEP_22_23_TOKEN_H

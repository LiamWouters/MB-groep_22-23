#include "Token.h"
#include <iostream>
#include <utility>

position::position(unsigned int l, unsigned int c) : line{l}, column{c} {}
token::token(std::string cont, unsigned int l, unsigned int c) : content{std::move(cont)}, pos{l, c}, type("UNKNOWN") {}
token::token(std::string cont, const position& p) : content{std::move(cont)}, pos{p}, type("UNKNOWN") {}
token::token(std::string cont, const position& p, std::string t)
    : content{std::move(cont)}, pos{p}, type{std::move(t)} {}

void token::print(std::ostream& out = std::cout) const {
    out << "(" << pos.line << ", " << pos.column << "): " << content << " [" << type << "]" << std::endl;
}
void token::reset() {
    content = "";
    pos = {1, 1};
    type = "UNKNOWN";
}

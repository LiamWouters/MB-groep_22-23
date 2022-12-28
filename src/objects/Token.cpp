#include "Token.h"
#include <iostream>
#include <utility>
#include <iomanip>

position::position(unsigned int l, unsigned int c) : line{l}, column{c} {}
token::token(std::string cont, unsigned int l, unsigned int c) : content{std::move(cont)}, pos{l, c}, type("UNKNOWN") {}
token::token(std::string cont, const position& p) : content{std::move(cont)}, pos{p}, type("UNKNOWN") {}
token::token(std::string cont, const position& p, std::string t)
    : content{std::move(cont)}, pos{p}, type{std::move(t)} {}
token::token(std::string cont, unsigned int l, unsigned int c,  std::string t) : content{std::move(cont)}, pos{l, c}, type(std::move(t)) {}

void token::print(std::ostream& out) const {
    out << std::left
    << std::setw(16) << "(" + std::to_string(pos.line) + ", " +  std::to_string(pos.column) + "):"
    << std::setw(40) << content
    << "[" + type + "]" << std::endl;
}
void token::reset() {
    content = "";
    pos = {1, 1};
    type = "UNKNOWN";
}

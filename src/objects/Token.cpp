#include "Token.h"
#include <utility>

position::position(unsigned int l, unsigned int c) : line{l}, column{c} {}
token::token(std::string cont, unsigned int l, unsigned int c) : content{std::move(cont)}, pos{l, c} {}
token::token(std::string cont, const position& p) : content{std::move(cont)}, pos{p} {}

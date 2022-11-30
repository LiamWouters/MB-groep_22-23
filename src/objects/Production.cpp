#include "Production.h"

#include "../utilities/utilities.h"

bool Production::operator<(const Production& lhs) const {
    if (this->head < lhs.head) {
        return true;
    }
    if (this->head > lhs.head) {
        return false;
    }
    if (this->body < lhs.body) {
        return true;
    }
    return false;
}

Production::Production(std::string h, std::vector<std::string> b) : head{std::move(h)}, body{std::move(b)} {}

std::string Production::toString() const {
    std::string result;
    result += this->head;
    result += " -> ";
    result += "`" + printVecDelimited(this->body, " ") + "`";
    return result;
}

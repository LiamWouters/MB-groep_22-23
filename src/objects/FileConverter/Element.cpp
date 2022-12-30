//
// Created by liam on 28/12/2022.
//

#include "Element.h"

Element::Element(const std::string &type) : type(type) {}

std::string Element::getType() const {
    return type;
}

Element::~Element() {}

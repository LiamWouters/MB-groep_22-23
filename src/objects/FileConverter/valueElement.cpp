//
// Created by liam on 28/12/2022.
//

#include "valueElement.h"

valueElement::valueElement() : Element("value") {
    name = "";
    value = "";
}

void valueElement::setName(const std::string &n) {
    name = n;
}

void valueElement::setValue(const std::string &v) {
    value = v;
}

std::string valueElement::writeElementToJSON() const {
    if (value == "") {
        std::cerr << "FILE CONVERSION ERROR: incomplete data element" << std::endl;
        return "ERROR";
    }

    if (name == "") {
        return value;
    }
    else {
        return name + ": " + value;
    }
}

valueElement::~valueElement() {}

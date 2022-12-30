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
        // moet komen van een EML, json kan niet iets zoals '"name": ' hebben
        return "\""+name+"\"" + ": \"\"";
    }
    return "\""+name+"\"" + ": " + value;
}

std::string valueElement::writeElementToJSONnoName() const {
    if (value == "") {
        return "\"\"";
    }
    return value;
}

std::string valueElement::writeElementToEML() const {
    std::string n = name;
    if (n == "") {
        n = "v";
    }
    return "["+n+"] "+value+" [/"+n+"]";
}

std::string valueElement::writeElementToEMLnoName() const {
    return value;
}

valueElement::~valueElement() {}

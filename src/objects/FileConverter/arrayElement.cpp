//
// Created by liam on 28/12/2022.
//

#include "arrayElement.h"

arrayElement::arrayElement() : containerElement("array") {
    name = "";
}

void arrayElement::setName(const std::string &n) {
    name = n;
}

std::string arrayElement::getName() const {
    return name;
}

void arrayElement::addElementToContainer(Element *e) {
    contents.emplace_back(e);
}

std::string arrayElement::writeElementToJSON() const {
    std::string json = "\""+name+"\"" + ": [";
    for (auto e : contents) {
        json += e->writeElementToJSONnoName();
        if (e != *(contents.end()-1)) {
            json += ", ";
        }
    }
    json += "]";
    return json;
}

std::string arrayElement::writeElementToJSONnoName() const {
    std::string json = "[";
    for (auto e : contents) {
        json += e->writeElementToJSONnoName();
        if (e != *(contents.end()-1)) {
            json += ", ";
        }
    }
    json += "]";
    return json;
}


std::string arrayElement::writeElementToEML() const {
    std::string eml;

    std::string n = name;
    if (n == "") {
        n = "a";
    }

    eml = "<"+n+"> ";
    for (auto e : contents) {
        eml += e->writeElementToEMLnoName();
        if (e != *(contents.end()-1)) {
            eml += ", ";
        }
    }
    eml += " </"+n+">";
    return eml;
}

std::string arrayElement::writeElementToEMLnoName() const {
    return writeElementToEML(); // for EML array should always print with name
}

arrayElement::~arrayElement() {
    for (auto e : contents) {
        delete e;
    }
}

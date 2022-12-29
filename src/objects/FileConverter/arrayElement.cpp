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

void arrayElement::addElementToContainer(Element *e) {
    contents.emplace_back(e);
}

std::string arrayElement::writeElementToJSON() const {
    std::string json;
    if (name != "") {
        json = name + ": [";
    }
    else {
        json = "[";
    }
    for (auto e : contents) {
        json += e->writeElementToJSON();
        if (e != *(contents.end()-1)) {
            json += ", ";
        }
    }
    json += "]";
    return json;
}

arrayElement::~arrayElement() {
    for (auto e : contents) {
        delete e;
    }
}

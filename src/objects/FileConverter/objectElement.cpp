//
// Created by liam on 28/12/2022.
//

#include "objectElement.h"

objectElement::objectElement() : containerElement("object") {
    name = "";
}

void objectElement::setName(const std::string &n) {
    name = n;
}

void objectElement::addElementToContainer(Element *e) {
    contents.emplace_back(e);
}

std::string objectElement::writeElementToJSON() const {
    std::string json;
    if (name != "") {
        json = name + ": {";
    }
    else {
        json = "{";
    }
    for (auto e : contents) {
        json += e->writeElementToJSON();
        if (e != *(contents.end()-1)) {
            json += ", ";
        }
    }
    json += "}";
    return json;
}

std::vector<Element*> objectElement::getContents() const {
    return contents;
}

objectElement::~objectElement() {
    for (auto e : contents) {
        delete e;
    }
}

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

std::string objectElement::getName() const {
    return name;
}

void objectElement::addElementToContainer(Element *e) {
    contents.emplace_back(e);
}

std::string objectElement::writeElementToJSON() const {
    std::string json = "\""+name+"\"" + ": {";

    for (auto e : contents) {
        json += e->writeElementToJSON();
        if (e != *(contents.end()-1)) {
            json += ", ";
        }
    }

    json += "}";
    return json;
}

std::string objectElement::writeElementToJSONnoName() const {
    std::string json = "{";

    for (auto e : contents) {
        json += e->writeElementToJSON();
        if (e != *(contents.end()-1)) {
            json += ", ";
        }
    }

    json += "}";
    return json;
}

std::string objectElement::writeElementToEML() const {
    std::string eml;

    std::string n = name;
    if (n == "") {
        n = "o";
    }

    eml = "["+n+"] ";
    for (auto e : contents) {
        eml += e->writeElementToEML() + " ";
    }
    eml += "[/"+n+"]";
    return eml;
}

std::string objectElement::writeElementToEMLnoName() const {
    return writeElementToEML(); // for EML objects should always print with name
}

std::vector<Element*> objectElement::getContents() const {
    return contents;
}

objectElement::~objectElement() {
    for (auto e : contents) {
        delete e;
    }
}

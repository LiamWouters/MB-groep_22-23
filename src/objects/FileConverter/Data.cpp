//
// Created by liam on 28/12/2022.
//

#include <sstream>
#include "Data.h"

Data::Data() {
    element = nullptr;
}

void Data::addElement(Element *elem, std::stack<containerElement*> &currentContainers) {
    if (currentContainers.empty() && element == nullptr) {
        element = elem;
    }
    else if (!currentContainers.empty()) {
        currentContainers.top()->addElementToContainer(elem);
    }
}

std::string Data::writeToJSON() const {
    if (element == nullptr) {return "";}

    std::stringstream json;
    if (element->getType() == "object") {
        // add elements to string
        objectElement* object = dynamic_cast<objectElement*>(element);
        if (object->getContents().size() == 1 && object->getName() == "root") {
            // for edge case: minimal: testinput eml1 -> json1
            json << object->getContents()[0]->writeElementToJSONnoName();
            return json.str();
        }
        json << "{\n\t";
        for (auto e : object->getContents()) {
            json << (*e).writeElementToJSON();
            if (e != *(object->getContents().end() - 1)) {
                json << ",\n\t";
            } else {
                json << "\n";
            }
        }
        json << "}";
    }
    else {
        json << element->writeElementToJSONnoName();
    }

    return json.str();
}

std::string Data::writeToEML() const {
    if (element == nullptr) {return "";}

    std::stringstream eml;
    if (element->getType() == "object") {
        objectElement* object = dynamic_cast<objectElement*>(element);
        std::string name = object->getName();
        if (name == "") {
            name = "root";
        }
        eml << "[" << name << "]\n\t";
        for (auto e : object->getContents()) {
            eml << (*e).writeElementToEML();
            if (e != *(object->getContents().end() - 1)) {
                eml << "\n\t";
            } else {
                eml << "\n";
            }
        }
        eml << "[/" << name << "]";
    }
    else {
        eml << "[root]\n\t" << element->writeElementToEML() << "\n[/root]";
    }
    return eml.str();
}

Data::~Data() {
    delete element;
}

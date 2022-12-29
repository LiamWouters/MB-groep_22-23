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
    std::stringstream json;
    if (element == nullptr) {return "";}
    if (element->getType() == "object") {
        // add elements to string
        objectElement* object = dynamic_cast<objectElement*>(element);
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
        json << element->writeElementToJSON();
    }

    return json.str();
}

std::string Data::writeToEML() const {
    std::cout << "WRITING TO EML NOT SUPPORTED YET" << std::endl;
    return "";
}

Data::~Data() {
    delete element;
}

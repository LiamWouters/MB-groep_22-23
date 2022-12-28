//
// Created by liam on 28/12/2022.
//

#include <sstream>
#include "Data.h"

Data::Data() {}

void Data::addElement(Element *element, std::stack<containerElement*> &currentContainers) {
    if (currentContainers.empty()) {
        elements.emplace_back(element);
    }
    else {
        currentContainers.top()->addElementToContainer(element);
    }
}

std::string Data::writeToJSON() const {
    // start string
    std::stringstream json;
    json << "{\n";

    // add elements to string
    for (auto e : elements) {
        json << (*e).writeElementToJSON();
        if (e != *(elements.end()-1)) {
            json << ",\n\t";
        } else {
            json << "\n";
        }
    }

    // end string
    json << "}";
    return json.str();
}

std::string Data::writeToEML() const {
    std::cout << "WRITING TO EML NOT SUPPORTED YET" << std::endl;
    return "";
}

Data::~Data() {
    for (auto e : elements) {
        delete e;
    }
}

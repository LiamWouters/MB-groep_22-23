//
// Created by liam on 28/12/2022.
//

#ifndef MB_GROEP_22_23_DATA_H
#define MB_GROEP_22_23_DATA_H

#include <iostream>
#include <vector>
#include <stack>
#include "valueElement.h"
#include "containerElement.h"
#include "arrayElement.h"
#include "objectElement.h"

class Data {
private:
    std::vector<Element*> elements;
public:
    Data();

    void addElement(Element* element, std::stack<containerElement*>& currentContainers);

    std::string writeToJSON() const;
    std::string writeToEML() const;

    virtual ~Data();
};


#endif //MB_GROEP_22_23_DATA_H

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
    Element* element; // contains the file, is either a container or a value
    /*
     * example: json file could be:
     *              {
     *                  "data": 2,
     *                  ...
     *              }
     *              OR
     *              5
     *              OR
     *              ["data", 1, ...]
     *              OR
     *              "string"
     *              OR
     *              ... (remaining value types)
     *          there is only 1 root element!
     *              => you can never have:
     *                      * example: {
     *                                      ...
     *                                 },
     *                                 {}
     */
public:
    Data();

    void addElement(Element* elem, std::stack<containerElement*>& currentContainers);

    std::string writeToJSON() const;
    std::string writeToEML() const;

    virtual ~Data();
};


#endif //MB_GROEP_22_23_DATA_H

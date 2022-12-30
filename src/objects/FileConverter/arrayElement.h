//
// Created by liam on 28/12/2022.
//

#ifndef MB_GROEP_22_23_ARRAYELEMENT_H
#define MB_GROEP_22_23_ARRAYELEMENT_H

#include <vector>
#include "containerElement.h"

class arrayElement : public containerElement {
    /*
     * Concrete class of container Element
     */
private:
    std::string name;
    std::vector<Element*> contents;
public:
    arrayElement();

    void setName(const std::string& n);
    std::string getName() const;
    void addElementToContainer(Element* e);
    std::string writeElementToJSON() const;
    std::string writeElementToEML() const;
    std::string writeElementToJSONnoName() const;
    std::string writeElementToEMLnoName() const;

    ~arrayElement();
};


#endif //MB_GROEP_22_23_ARRAYELEMENT_H

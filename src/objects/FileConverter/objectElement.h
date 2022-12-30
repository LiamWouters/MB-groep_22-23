//
// Created by liam on 28/12/2022.
//

#ifndef MB_GROEP_22_23_OBJECTELEMENT_H
#define MB_GROEP_22_23_OBJECTELEMENT_H

#include <vector>
#include "containerElement.h"

class objectElement : public containerElement {
    /*
     * Concrete class of containerElement
     */
private:
    std::string name;
    std::vector<Element*> contents;
public:
    objectElement();

    void setName(const std::string& n);
    std::string getName() const;
    void addElementToContainer(Element* e);
    std::vector<Element*> getContents() const;
    std::string writeElementToJSON() const;
    std::string writeElementToEML() const;
    std::string writeElementToJSONnoName() const;
    std::string writeElementToEMLnoName() const;

    ~objectElement();
};


#endif //MB_GROEP_22_23_OBJECTELEMENT_H

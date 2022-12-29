//
// Created by liam on 28/12/2022.
//

#ifndef MB_GROEP_22_23_CONTAINERELEMENT_H
#define MB_GROEP_22_23_CONTAINERELEMENT_H

#include "Element.h"

class containerElement : public Element {
    /*
     * abstract class (concrete classes are available for array and object)
     */
public:
    containerElement(const std::string &type);
    virtual void addElementToContainer(Element* e)=0;
    virtual ~containerElement();
};


#endif //MB_GROEP_22_23_CONTAINERELEMENT_H

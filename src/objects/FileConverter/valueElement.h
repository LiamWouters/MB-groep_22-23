//
// Created by liam on 28/12/2022.
//

#ifndef MB_GROEP_22_23_VALUEELEMENT_H
#define MB_GROEP_22_23_VALUEELEMENT_H

#include "Element.h"

class valueElement : public Element{
    /*
     * Concrete class of Element: covers STRING, NUMBER, BOOLEAN and NULL
     */
private:
    std::string name;
    std::string value; // value in string form
public:
    valueElement();
    void setName(const std::string& n);
    void setValue(const std::string& v);
    std::string writeElementToJSON() const;
};


#endif //MB_GROEP_22_23_VALUEELEMENT_H

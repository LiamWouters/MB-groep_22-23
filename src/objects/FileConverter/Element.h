//
// Created by liam on 28/12/2022.
//

#ifndef MB_GROEP_22_23_ELEMENT_H
#define MB_GROEP_22_23_ELEMENT_H

#include <iostream>

class Element {
    /*
     * Abstract class for generated data structure
     */
private:
    std::string type;
public:
    Element(const std::string &type);
    std::string getType() const;
    virtual std::string writeElementToJSON() const =0;
    virtual std::string writeElementToEML() const =0;
    virtual std::string writeElementToJSONnoName() const =0;
    virtual std::string writeElementToEMLnoName() const =0;
    virtual ~Element();
};


#endif //MB_GROEP_22_23_ELEMENT_H

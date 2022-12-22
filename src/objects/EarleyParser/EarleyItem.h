//
// Created by michel on 22.12.22.
//

#ifndef MB_GROEP_22_23_EARLEYITEM_H
#define MB_GROEP_22_23_EARLEYITEM_H

#include "../Production.h"

class EarleyItem {
public:
    /// constructors
    EarleyItem(Production production, unsigned int dot, unsigned int start);

    /// memberfunctions
    bool isCompleted() const;
    std::string getNextSymbol() const;

    /// print functions
    void print(std::ostream& out) const;

    /// operators
    bool operator==(const EarleyItem& rhs) const;

    /// variables
    Production m_production;
    unsigned int m_dot;
    unsigned int m_start;
};

#endif // MB_GROEP_22_23_EARLEYITEM_H

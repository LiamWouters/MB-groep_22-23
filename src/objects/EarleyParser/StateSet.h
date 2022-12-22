//
// Created by michel on 22.12.22.
//

#ifndef MB_GROEP_22_23_STATESET_H
#define MB_GROEP_22_23_STATESET_H

#include "EarleyItem.h"
#include <vector>

class StateSet {
public:
    /// member functions
    void addEarleyItem(const EarleyItem& earleyItem);
    void addEarleyItemUnsafe(const EarleyItem& earleyItem);

    /// print function
    void print(std::ostream& out) const;

    /// variables
    std::vector<EarleyItem> m_set;
};

#endif // MB_GROEP_22_23_STATESET_H

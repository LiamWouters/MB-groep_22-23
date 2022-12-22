//
// Created by michel on 22.12.22.
//

#ifndef MB_GROEP_22_23_STATESET_H
#define MB_GROEP_22_23_STATESET_H

#include "EarleyItem.h"
#include <vector>

class StateSet {
public:
    std::vector<StateSet> m_set;
};

#endif // MB_GROEP_22_23_STATESET_H

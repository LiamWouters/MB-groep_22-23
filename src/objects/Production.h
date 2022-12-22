#ifndef CFG_PRODUCTION_H
#define CFG_PRODUCTION_H

#include <string>
#include <vector>

class Production {
public:
    Production(std::string h, std::vector<std::string> b);

    std::string head;
    std::vector<std::string> body;

    bool operator<(const Production& lhs) const;

    bool operator==(const Production& rhs) const;

    std::string toString() const;
};

#endif // CFG_PRODUCTION_H

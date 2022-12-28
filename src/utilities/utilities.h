#ifndef CFG_UTILITIES_H
#define CFG_UTILITIES_H

#include <set>
#include <string>
#include <vector>

std::vector<std::string> sortVector(const std::vector<std::string>& vec);

std::string printVecDelimited(const std::vector<std::string>& vec, const std::string& delimiter);

std::string printSetDelimited(const std::set<std::string>& set, const std::string& delimiter);

std::string printSetOfPairsDelimited(const std::set<std::pair<std::string, std::string>>& set,
                                     const std::string& delimiter);

std::string printBrackets(const std::vector<std::string>& vec);

bool stringInSet(const std::string& symbol, const std::set<std::string>& symbols);

bool stringInVector(const std::string& symbol, const std::vector<std::string>& symbols);

bool vectorContainsOnly(const std::vector<std::string>& body, const std::set<std::string>& symbols);

std::set<std::string> setIntersection(const std::set<std::string>& a, const std::set<std::string>& b);

std::vector<std::string> stringToVector(const std::string& s);

std::string stringForSpecialCharacters(const std::string& s);

bool charInSet(const std::set<char>& s, const char ch);

bool stringContainsInvalidChars(const std::set<char>& valid_chars, const std::string& s);

#endif // CFG_UTILITIES_H

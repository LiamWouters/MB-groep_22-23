#include "utilities.h"
#include <algorithm>

std::vector<std::string> sortVector(const std::vector<std::string>& vec) {
    std::vector<std::string> result = vec;
    std::sort(result.begin(), result.end());
    return result;
}

std::string printVecDelimited(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::string result;
    std::string separator;
    for (const auto& x : vec) {
        result += separator;
        result += x;
        separator = delimiter;
    }
    return result;
}

std::string printSetDelimited(const std::set<std::string>& set, const std::string& delimiter) {
    std::string result;
    std::string separator;
    result += "{";
    for (const auto& x : set) {
        result += separator;
        result += x;
        separator = delimiter;
    }
    result += "}";
    return result;
}

std::string printSetOfPairsDelimited(const std::set<std::pair<std::string, std::string>>& set,
                                     const std::string& delimiter) {
    std::string result;
    std::string separator;
    result += "{";
    for (const auto& x : set) {
        result += separator;
        result += "(";
        result += x.first;
        result += ", ";
        result += x.second;
        result += ")";
        separator = delimiter;
    }
    result += "}";
    return result;
}

std::string printBrackets(const std::vector<std::string>& vec) {
    std::string result;
    std::vector<std::string> sortedVec = sortVector(vec);
    result += "{";
    result += printVecDelimited(sortedVec, ", ");
    result += "}";
    return result;
}

bool stringInSet(const std::string& symbol, const std::set<std::string>& symbols) {
    if (symbols.find(symbol) != symbols.end()) {
        return true;
    }
    return false;
};

bool stringInVector(const std::string& symbol, const std::vector<std::string>& symbols) {
    if (find(symbols.begin(), symbols.end(), symbol) != symbols.end()) {
        return true;
    }
    return false;
}

bool vectorContainsOnly(const std::vector<std::string>& body, const std::set<std::string>& symbols) {
    for (const auto& x : body) {
        if (symbols.find(x) == symbols.end()) {
            return false;
        }
    }
    return true;
}

std::set<std::string> setIntersection(const std::set<std::string>& a, const std::set<std::string>& b) {
    std::set<std::string> out;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(out, out.begin()));
    return out;
}

std::vector<std::string> stringToVector(const std::string& s) {
    std::vector<std::string> result;
    for (const auto& ch : s) {
        result.emplace_back(std::string(1, ch));
    }
    return result;
}

std::string stringForSpecialCharacters(const std::string& s) {
    if (s == "\n") {
        return "\\n";
    }
    if (s == "\t") {
        return "\\t";
    }
    if (s == " ") {
        return "\" \"";
    }
    return s;
};

bool charInSet(const std::set<char>& s, const char ch) { return s.find(ch) != s.end(); }

bool stringContainsInvalidChars(const std::set<char>& valid_chars, const std::string& s) {
    for (unsigned int i = 0; i < s.size(); i++) {
        if (!charInSet(valid_chars, s[i])) {
            if (i == 0 or i == s.size() - 1) {
                continue;
            }
            return true;
        }
    }
    return false;
}

void insertStringInVectorIfLastIsDifferent(std::vector<std::string>& vec, const std::string& to_insert) {
    if (vec.empty()) {
        vec.emplace_back(to_insert);
    } else {
        if (vec.back() != to_insert) {
            vec.emplace_back(to_insert);
        }
    }
};
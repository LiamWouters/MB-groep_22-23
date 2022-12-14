#include "JsonTokenizer.h"
#include <fstream>
#include <iostream>

void JsonTokenizer::tokenize(const std::string& path) {
    char ch;
    // lines and columns start at 1
    unsigned int l = 1, c = 1;
    std::ifstream infile(path);
    if (!infile.is_open())
        throw std::runtime_error("Could not open file: '" + path + "'\n");

    // read input without skipping whitespaces
    while (infile >> std::noskipws >> ch) {
        tokens.emplace_back(std::string(1, ch), l, c);
        if (ch == '\n') {
            c = 1;
            l++;
        } else {
            c++;
        }
    }
}

void JsonTokenizer::split(const std::string& path) {
    char ch;
    std::ifstream infile(path);
    if (!infile.is_open())
        throw std::runtime_error("Could not open file: '" + path + "'\n");

    while (infile >> std::noskipws >> ch) {
        strings.emplace_back(1, ch);
    }
}

void JsonTokenizer::printTokens(std::ostream& out = std::cout) const {
    for (auto& cur : tokens) {
        cur.print(out);
    }
}

void JsonTokenizer::printStrings(std::ostream& out) const {
    for (auto& cur : strings) {
        out << cur << std::endl;
    }
}

#include "EMLTokenizer.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <stack>

#include "../utilities/utilities.h"

EMLTokenizer::EMLTokenizer() { generateAllowedStringCharacters(); }

void EMLTokenizer::tokenize(const std::string& path) {
    tokens.clear();
    char ch;
    // lines and columns start at 1
    unsigned int l = 1, c = 1;
    std::ifstream infile(path);
    if (!infile.is_open())
        throw std::runtime_error("Could not open file: '" + path + "'\n");

    // read input without skipping whitespaces
    while (infile >> std::noskipws >> ch) {
        tokens.emplace_back(std::string(1, ch), l, c, std::string(1, ch));
        increaseRow(l, c, ch);
    }
    infile.close();
}

void EMLTokenizer::split(const std::string& path) {
    strings.clear();
    char ch;
    std::ifstream infile(path);
    if (!infile.is_open())
        throw std::runtime_error("Could not open file: '" + path + "'\n");

    while (infile >> std::noskipws >> ch) {
        strings.emplace_back(1, ch);
    }
    infile.close();
}

void EMLTokenizer::printTokens(std::ostream& out) const {
    for (auto& cur : tokens) {
        cur.print(out);
    }
}

void EMLTokenizer::printTokensToFile(const std::string& path) const {
    std::ofstream file(path);
    if (file.is_open()) {
        printTokens(file);
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

void EMLTokenizer::printStrings(std::ostream& out) const {
    for (auto& cur : strings) {
        out << cur << std::endl;
    }
}

void EMLTokenizer::tokenizeString(const std::string& input) {
    unsigned int c = 1;
    for (const auto& ch : input) {
        tokens.emplace_back(std::string(1, ch), 1, c);
        c++;
    }
}
void EMLTokenizer::reset() {
    tokens.clear();
    strings.clear();
}

void EMLTokenizer::tokenizeSimplified(const std::string& path) {
    tokens.clear();

    std::stack<std::string> tagStack;

    bool inTag = false;

    bool inString = false;
    bool inNumber = false;

    bool inFalse = false;
    int counter_false = 0;
    position saved_false = {1, 1};

    bool inTrue = false;
    int counter_true = 0;
    position saved_true = {1, 1};

    bool inNull = false;
    int counter_null = 0;
    position saved_null = {1, 1};

    std::string temp_false;
    std::string temp_true;
    std::string temp_null;

    char ch;
    // lines and columns start at 1
    unsigned int l = 1, c = 1;
    std::ifstream infile(path);
    if (!infile.is_open())
        throw std::runtime_error("Could not open file: '" + path + "'\n");

    token t("", 1, 1);
    // read input without skipping whitespaces
    while (infile >> std::noskipws >> ch) {

        // detect if token is tag
        if (ch == '[' and not inTag) {
            inTag = true;
            t.content += ch;
            t.pos = {l, c};
            increaseRow(l, c, ch);
            continue;
        }

        // detect if token is array-tag
        if (ch == '<' and not inTag) {
            inTag = true;
            t.content += ch;
            t.pos = {l, c};
            increaseRow(l, c, ch);
            continue;
        }

        if (ch == ']' and inTag) {
            inTag = false;
            t.content += ch;
            std::string tagName;
            if (t.content[0] == '<') {
                t.type = "INVALID_TAG";
                if (t.content[1] == '/') {
                    if (!tagStack.empty()){
                        tagStack.pop();
                    }
                } else {
                    tagStack.push("");
                }
            }
            if (t.content[1] == '/') {
                tagName = t.content.substr(2, t.content.size()-3);
                if (tagName == tagStack.top()) {
                    if (tagName == "root") {
                        t.type = "ROOT_CLOSE";
                    } else {
                        t.type = "TAG_CLOSE";
                    }
                } else {
                    t.type = "UNMATCHING_TAG";
                }
                if (!tagStack.empty()){
                    tagStack.pop();
                }
            } else {
                tagName = t.content.substr(1, t.content.size()-2);
                tagStack.push(tagName);
                if (tagName == "root") {
                    t.type = "ROOT_OPEN";
                } else {
                    t.type = "TAG_OPEN";
                }
            }
            tokens.emplace_back(t);
            t.reset();
            increaseRow(l, c, ch);
            continue;
        }

        if (ch == '>' and inTag) {
            inTag = false;
            t.content += ch;
            std::string tagName;
            if (t.content[0] == '[') {
                t.type = "INVALID_TAG";
                if (t.content[1] == '/') {
                    if (!tagStack.empty()){
                        tagStack.pop();
                    }
                } else {
                    tagStack.push("");
                }
            }
            if (t.content[1] == '/') {
                tagName = t.content.substr(2, t.content.size()-3);
                if (tagName == tagStack.top()) {
                    t.type = "ARRAY_TAG_CLOSE";
                } else {
                    t.type = "UNMATCHING_TAG";
                }
                if (!tagStack.empty()){
                    tagStack.pop();
                }
            } else {
                tagName = t.content.substr(1, t.content.size()-2);
                tagStack.push(tagName);
                t.type = "ARRAY_TAG_OPEN";
            }
            tokens.emplace_back(t);
            t.reset();
            increaseRow(l, c, ch);
            continue;
        }

        if (inTag) {
            if (ch == '\n') {
                inTag = false;
                t.type = "INVALID_TAG";
                tokens.emplace_back(t);
                t.reset();
            } else {
                t.content += ch;
            }
            increaseRow(l, c, ch);
            continue;
        }

        // detect if token is string
        if (ch == '\"') {
            if (inString) {
                inString = false;
                t.content += ch;
                t.type = "STRING";
                tokens.emplace_back(t);
                t.reset();
                increaseRow(l, c, ch);
                continue;
            } else {
                inString = true;
                t.content += ch;
                t.pos = {l, c};
                increaseRow(l, c, ch);
                continue;
            }
        }

        // keep adding characters to string, but if character is \n escape string and finish token
        if (inString) {
            if (ch == '\n') {
                inString = false;
                t.type = "INVALID_STRING_MISSING_QUOTE";
                tokens.emplace_back(t);
                t.reset();
            } else {
                t.content += ch;
            }
            increaseRow(l, c, ch);
            continue;
        }

        // detect if token is number
        if ((ch == '-' or (ch >= '0' and ch <= '9')) and !inNumber) {
            inNumber = true;
            t.content = ch;
            t.pos = {l, c};
            t.type = "NUMBER";
            increaseRow(l, c, ch);
            continue;
        }

        if (inNumber and
            (((ch >= '0' and ch <= '9')) or ch == '.' or ch == 'e' or ch == 'E' or ch == '+' or ch == '-')) {
            t.content += ch;
            increaseRow(l, c, ch);
            continue;
        }

        if (inNumber and
            !(((ch >= '0' and ch <= '9')) or ch == '.' or ch == '+' or ch == '-' or ch == 'e' or ch == 'E')) {
            inNumber = false;
            tokens.emplace_back(t);
            t.reset();
        }

        if (ch == ' ' or ch == '\t' or ch == '\n') {
            increaseRow(l, c, ch);
            continue;
        }

        // false
        if (ch == 'f') {
            inFalse = true;
            saved_false = {l, c};
            temp_false = "";
        }
        if (inFalse and counter_false < 5) {
            temp_false += ch;
            counter_false++;
        }
        if (counter_false == 5) {
            if (temp_false == "false") {
                for (int i = 0; i < 4; i++) {
                    tokens.pop_back();
                }
                t.content = "false";
                t.type = "BOOLEAN";
                t.pos = saved_false;

                tokens.emplace_back(t);
                t.reset();
                increaseRow(l, c, ch);

                counter_false = 0;
                inFalse = false;
                temp_false = "";
                continue;
            }
            counter_false = 0;
            inFalse = false;
            temp_false = "";
        }

        // true
        if (ch == 't') {
            inTrue = true;
            saved_true = {l, c};
            temp_true = "";
        }
        if (inTrue and counter_true < 4) {
            temp_true += ch;
            counter_true++;
        }
        if (counter_true == 4) {
            if (temp_true == "true") {
                for (int i = 0; i < 3; i++) {
                    tokens.pop_back();
                }
                t.content = "true";
                t.type = "BOOLEAN";
                t.pos = saved_true;

                tokens.emplace_back(t);
                t.reset();
                increaseRow(l, c, ch);

                counter_true = 0;
                inTrue = false;
                temp_true = "";
                continue;
            }
            counter_true = 0;
            inTrue = false;
            temp_true = "";
        }

        // null
        if (ch == 'n') {
            inNull = true;
            saved_null = {l, c};
            temp_null = "";
        }
        if (inNull and counter_null < 4) {
            temp_null += ch;
            counter_null++;
        }
        if (counter_null == 4) {
            if (temp_null == "null") {
                for (int i = 0; i < 3; i++) {
                    tokens.pop_back();
                }
                t.content = "null";
                t.type = "NULL";
                t.pos = saved_null;

                tokens.emplace_back(t);
                t.reset();
                increaseRow(l, c, ch);

                counter_null = 0;
                inNull = false;
                temp_null = "";
                continue;
            }
            counter_null = 0;
            inNull = false;
            temp_null = "";
        }

        // handle single characters
        if (ch == ',') {
            t.type = "COMMA";
        }
        t.content = ch;
        t.pos = {l, c};
        tokens.emplace_back(t);
        t.reset();

        increaseRow(l, c, ch);
        // count rows and columns
    }
    if (inString) {
        t.type = "INVALID_STRING_MISSING_QUOTE";
        tokens.emplace_back(t);
    } else if (inNumber) {
        tokens.emplace_back(t);
    }
    fixNumberTokens();
    fixStringTokens();
    infile.close();
}

void EMLTokenizer::increaseRow(unsigned int& l, unsigned int& c, char ch) const {
    if (ch == '\n') {
        c = 1;
        l++;
    } else {
        c++;
    }
}

void EMLTokenizer::fixNumberTokens() {
    for (auto& cur : tokens) {
        std::string type = cur.type;
        if (type == "NUMBER") {
            if (!isJsonNumber(cur.content)) {
                cur.type = "INVALID_NUMBER";
            }
        }
    }
}

/*
bool EMLTokenizer::isDouble(const std::string& s) const {
    char* end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0';
}
 */

bool EMLTokenizer::isJsonNumber(const std::string& s) const {
    if (s.empty()) {
        return false;
    }
    bool foundExponent = false;
    bool foundSignAfterExponent = false;
    bool startsWithZero = false;
    bool endsWithDigit = false;
    bool foundDot = false;
    bool startsWithSign = false;

    for (unsigned int i = 0; i < s.size(); i++) {
        char cur_char = s[i];
        endsWithDigit = false;

        bool curCharIsDigit = std::isdigit(cur_char);

        if (i == 0) {
            // number must start with - or digit
            if (!curCharIsDigit and cur_char != '-')
                return false;
            // check if starts with 0
            if (cur_char == '0')
                startsWithZero = true;
            // check if ends with digit
            if (cur_char != '-')
                endsWithDigit = true;
            if (cur_char == '-')
                startsWithSign = true;
        } else {
            // if the number starts with 0
            if (startsWithSign) {
                if (!curCharIsDigit) {
                    return false;
                } else {
                    endsWithDigit = true;
                    startsWithSign = false;
                    if (cur_char == '0') {
                        startsWithZero = true;
                    }
                }
            } else if (startsWithZero) {
                // since there is a second digit after 0, the number can not have started with 0
                startsWithZero = false;
                if (cur_char != '.') {
                    // if it started with 0, next char must be a dot
                    return false;
                } else {
                    // if it is a dot, set foundDot to true
                    foundDot = true;
                }
            } else if (cur_char == '.') {
                if (foundExponent) {
                    return false;
                }
                // there may not be multiple dots
                if (foundDot) {
                    return false;
                } else {
                    foundDot = true;
                }
            } else if (cur_char == 'e' or cur_char == 'E') {
                if (foundExponent) {
                    // there may not be multiple E, e
                    return false;
                } else {
                    foundExponent = true;
                }
            } else if (foundExponent and !foundSignAfterExponent) {
                // next char must be exponent
                if (!curCharIsDigit and cur_char != '+' and cur_char != '-') {
                    return false;
                }
                // check if sign after dot
                if (cur_char == '+' or cur_char == '-') {
                    foundSignAfterExponent = true;
                } else {
                    endsWithDigit = true;
                }
            } else {
                // there is no sign found after exponent
                foundSignAfterExponent = false;
                // cur char must be a digit
                if (!curCharIsDigit) {
                    return false;
                } else {
                    endsWithDigit = true;
                }
            }
        }
    }
    // number must end with a digit
    if (!endsWithDigit) {
        return false;
    }
    return true;
}

void EMLTokenizer::fixStringTokens() {
    for (auto& cur : tokens) {
        std::string type = cur.type;
        if (type == "STRING") {
            if (stringContainsInvalidChars(m_allowed_string_characters, cur.content)) {
                cur.type = "INVALID_STRING_DISALLOWED_CHARACTERS";
            }
        }
    }
}

void EMLTokenizer::generateAllowedStringCharacters() {
    for (char ch = '0'; ch <= '9'; ch++) {
        m_allowed_string_characters.insert(ch);
    }
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        m_allowed_string_characters.insert(ch);
    }
    for (char ch = 'a'; ch <= 'z'; ch++) {
        m_allowed_string_characters.insert(ch);
    }
    m_allowed_string_characters.insert('?');
    m_allowed_string_characters.insert('!');
    m_allowed_string_characters.insert('.');
    m_allowed_string_characters.insert('+');
    m_allowed_string_characters.insert('-');
    m_allowed_string_characters.insert('(');
    m_allowed_string_characters.insert(')');
    m_allowed_string_characters.insert(' ');
    m_allowed_string_characters.insert('_');
}
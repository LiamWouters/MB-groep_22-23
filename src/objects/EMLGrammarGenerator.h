#ifndef MB_GROEP_22_23_EMLGRAMMARGENERATOR_H
#define MB_GROEP_22_23_EMLGRAMMARGENERATOR_H

#include "../../lib/nlohmann-json/json.hpp"
#include <string>

class EMLGrammarGenerator {
public:
    static void generate(); // generate the eml grammar in res/eml_grammar.json, based on
                            // the simplified json form specified in "https://www.json.org/json-en.html"
                            // simplifications:
                            // a character can only be: a . z, A . Z, 0 . 9, '.', '?', '!', '+', '-', '
                            // '
                            // a ws can not be: \r
    static void generateSimplified();
private:
    static void generateStart(nlohmann::json& j);   // generate start symbol
    static void generateVariables(nlohmann::json& j);   // generate all variables for json
    static void generateTerminals(nlohmann::json& j);   // generate all terminals for json
    static void generateProductions(nlohmann::json& j); // generate all productions for json
    static void addProduction(nlohmann::json& j, const std::string& head,
                              const std::vector<std::string>& body); // add one production to json j
    static void addProductions(
        nlohmann::json& j, const std::string& head,
        const std::vector<std::vector<std::string>>& bodies); // add multiple productions to json j
    static void addProductionsCharacter(nlohmann::json& j);   // add all productions for "character" to json j
    static void addProductionsOneNine(nlohmann::json& j);     // add all productions for "onenine" to json j
};

#endif // MB_GROEP_22_23_EMLGRAMMARGENERATOR_H

#include <fstream>
#include <iomanip>
#include <iostream>

#include "JsonGrammarGenerator.h"

void JsonGrammarGenerator::generate() {
    // fixed path
    std::ofstream file("../res/json_grammar.json");

    nlohmann::json j;
    generateStart(j);
    generateVariables(j);
    generateTerminals(j);
    generateProductions(j);

    file << std::setw(4) << j;
    file.close();
}

void JsonGrammarGenerator::generateStart(nlohmann::json& j) { j["Start"] = "json"; }

void JsonGrammarGenerator::generateVariables(nlohmann::json& j) {
    j["Variables"] = {"json",    "value",   "object",     "members",   "member", "array",   "elements",
                      "element", "string",  "characters", "character", "number", "integer", "digits",
                      "digit",   "onenine", "fraction",   "exponent",  "sign",   "ws"};
}

void JsonGrammarGenerator::generateTerminals(nlohmann::json& j) {
    j["Terminals"] = {"{", "}", ",", ":", "[", "]", "-", ".", "+", " ", "\n", "\t", "\""};
    for (char ch = '0'; ch <= '9'; ch++) {
        j["Terminals"].emplace_back(std::string(1, ch));
    }
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        j["Terminals"].emplace_back(std::string(1, ch));
    }
    for (char ch = 'a'; ch <= 'z'; ch++) {
        j["Terminals"].emplace_back(std::string(1, ch));
    }
    j["Terminals"].emplace_back("?");
    j["Terminals"].emplace_back("!");
    j["Terminals"].emplace_back("(");
    j["Terminals"].emplace_back(")");
}

void JsonGrammarGenerator::generateProductions(nlohmann::json& j) {
    addProduction(j, "json", {"element"});
    addProductions(j, "value",
                   {{"object"},
                    {"array"},
                    {"string"},
                    {"number"},
                    {"t", "r", "u", "e"},
                    {"f", "a", "l", "s", "e"},
                    {"n", "u", "l", "l"}});
    addProductions(j, "object", {{"{", "ws", "}"}, {"{", "members", "}"}});
    addProductions(j, "members", {{"member"}, {"member", ",", "members"}});
    addProduction(j, "member", {"ws", "string", "ws", ":", "element"});
    addProductions(j, "array", {{"[", "ws", "]"}, {"[", "elements", "]"}});
    addProductions(j, "elements", {{"element"}, {"element", ",", "elements"}});
    addProduction(j, "element", {"ws", "value", "ws"});
    addProduction(j, "string", {"\"", "characters", "\""});
    addProductions(j, "characters", {{}, {"character", "characters"}});
    addProductionsCharacter(j);
    addProduction(j, "number", {"integer", "fraction", "exponent"});
    addProductions(j, "integer", {{"digit"}, {"onenine", "digits"}, {"-", "digit"}, {"-", "onenine", "digits"}});
    addProductions(j, "digits", {{"digit"}, {"digit", "digits"}});
    addProductions(j, "digit", {{"0"}, {"onenine"}});
    addProductionsOneNine(j);
    addProductions(j, "fraction", {{}, {".", "digits"}});
    addProductions(j, "exponent", {{}, {"E", "sign", "digits"}, {"e", "sign", "digits"}});
    addProductions(j, "sign", {{}, {"+"}, {"-"}});

    // addProductions(j, "ws", {{}, {" "}, {"\n"}, {"\t"}});

    // CHANGED PRODUCTION
    addProductions(j, "ws", {{}, {" ", "ws"}, {"\n", "ws"}, {"\t", "ws"}});
}

void JsonGrammarGenerator::addProduction(nlohmann::json& j, const std::string& head,
                                         const std::vector<std::string>& body) {
    nlohmann::json object;
    object["head"] = head;
    object["body"] = body;
    j["Productions"].emplace_back(object);
}
void JsonGrammarGenerator::addProductions(nlohmann::json& j, const std::string& head,
                                          const std::vector<std::vector<std::string>>& bodies) {
    for (auto& body : bodies) {
        addProduction(j, head, body);
    }
}
void JsonGrammarGenerator::addProductionsCharacter(nlohmann::json& j) {
    for (char ch = '0'; ch <= '9'; ch++) {
        addProduction(j, "character", {std::string(1, ch)});
        ;
    }
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        addProduction(j, "character", {std::string(1, ch)});
        ;
    }
    for (char ch = 'a'; ch <= 'z'; ch++) {
        addProduction(j, "character", {std::string(1, ch)});
        ;
    }
    addProductions(j, "character", {{"?"}, {"!"}, {"("}, {")"}, {"."}, {"+"}, {"-"}, {" "}, {","}, {":"}});
}
void JsonGrammarGenerator::addProductionsOneNine(nlohmann::json& j) {
    for (char ch = '1'; ch <= '9'; ch++) {
        addProduction(j, "onenine", {std::string(1, ch)});
        ;
    }
}

void JsonGrammarGenerator::simplifiedGenerate() {
    std::ofstream file("../res/json_grammar_simplified.json");

    nlohmann::json j;

    j["Start"] = "json";
    j["Variables"] = {"json", "element", "value", "object", "array", "members", "member", "elements"};
    j["Terminals"] = {"ARRAY_OPEN", "ARRAY_CLOSE", "CURLY_OPEN", "CURLY_CLOSE", "COLON",
                      "COMMA",      "STRING",      "NUMBER",     "BOOLEAN",     "NULL"};

    addProduction(j, "json", {"element"});
    addProductions(j, "value", {{"object"}, {"array"}, {"STRING"}, {"NUMBER"}, {"BOOLEAN"}, {"NULL"}});
    addProductions(j, "object", {{"CURLY_OPEN", "CURLY_CLOSE"}, {"CURLY_OPEN", "members", "CURLY_CLOSE"}});
    addProductions(j, "members", {{"member"}, {"member", "COMMA", "members"}});
    addProduction(j, "member", {"STRING", "COLON", "element"});
    addProductions(j, "array", {{"ARRAY_OPEN", "ARRAY_CLOSE"},{"ARRAY_OPEN", "elements", "ARRAY_CLOSE"}});
    addProductions(j, "elements", {{"element"}, {"element", "COMMA", "elements"}});
    addProduction(j, "element", {"value"});

    file << std::setw(4) << j;
    file.close();
}

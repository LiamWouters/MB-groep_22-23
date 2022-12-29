#include <fstream>
#include <iomanip>
#include <iostream>

#include "EMLGrammarGenerator.h"

void EMLGrammarGenerator::generate() {
    // fixed path
    std::ofstream file("../res/eml_grammar.json");

    nlohmann::json j;
    generateStart(j);
    generateVariables(j);
    generateTerminals(j);
    generateProductions(j);

    file << std::setw(4) << j;
    file.close();
}

void EMLGrammarGenerator::generateStart(nlohmann::json& j) { j["Start"] = "eml"; }

void EMLGrammarGenerator::generateVariables(nlohmann::json& j) {
    j["Variables"] = {"eml",    "value",   "object",     "members",   "member", "array",   "elements",
                      "element", "OTag", "CTag", "string",  "characters", "character", "number", "integer", "digits",
                      "digit",   "onenine", "fraction",   "exponent",  "sign",   "ws"};
}

void EMLGrammarGenerator::generateTerminals(nlohmann::json& j) {
    j["Terminals"] = {"<", ">", "/", "-", ".", "+", " ", "\n", "\t"};
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

void EMLGrammarGenerator::generateProductions(nlohmann::json& j) {
    addProduction(j, "eml", {"<", "r", "o", "o", "t", ">", "element", "<", "/", "r", "o", "o", "t", ">"});
    addProductions(j, "value",
                   {{"object"},
                    {"array"},
                    {"string"},
                    {"number"},
                    {"t", "r", "u", "e"},
                    {"f", "a", "l", "s", "e"},
                    {"n", "u", "l", "l"}});
    addProductions(j, "object", {{"OTag", "ws", "CTag"}, {"OTag", "members", "CTag"}});
    addProductions(j, "members", {{"member"}, {"member", "members"}});
    addProduction(j, "member", {"element"});
    addProductions(j, "array", {{"ws"}, {"elements"}});
    addProductions(j, "elements", {{"element"}, {"element", "elements"}});
    addProduction(j, "element", {"ws", "value", "ws"});
    addProduction(j, "OTag", {"<", "string", ">"});
    addProduction(j, "CTag", {"<", "/", "string", ">"});
    addProduction(j, "string", {"characters"});
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

void EMLGrammarGenerator::addProduction(nlohmann::json& j, const std::string& head,
                                         const std::vector<std::string>& body) {
    nlohmann::json object;
    object["head"] = head;
    object["body"] = body;
    j["Productions"].emplace_back(object);
}
void EMLGrammarGenerator::addProductions(nlohmann::json& j, const std::string& head,
                                          const std::vector<std::vector<std::string>>& bodies) {
    for (auto& body : bodies) {
        addProduction(j, head, body);
    }
}
void EMLGrammarGenerator::addProductionsCharacter(nlohmann::json& j) {
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
void EMLGrammarGenerator::addProductionsOneNine(nlohmann::json& j) {
    for (char ch = '1'; ch <= '9'; ch++) {
        addProduction(j, "onenine", {std::string(1, ch)});
        ;
    }
}
void EMLGrammarGenerator::generateSimplified(){
    std::ofstream file("../res/eml_grammar_simplified.json");

    nlohmann::json j;

    j["Start"] = "eml";
    j["Variables"] = {"eml", "value", "object", "members", "member", "array", "elements", "element", "opentag", "closetag"};
    j["Terminals"] = {"ROOT_OPEN", "ROOT_CLOSE", "OPEN_BRACK", "CLOSE_BRACK", "SLASH", "STRING", "NUMBER", "BOOLEAN", "NULL"};

    addProduction(j, "eml", {"ROOT_OPEN", "element", "ROOT_CLOSE"});
    addProductions(j, "value", {{"object"}, {"array"}, {"STRING"}, {"NUMBER"}, {"BOOLEAN"}, {"NULL"}});
    addProductions(j, "object", {{"opentag", "closetag"}, {"opentag", "members", "closetag"}});
    addProductions(j, "opentag", {{"OPEN_BRACK", "STRING", "CLOSE_BRACK"}});
    addProductions(j, "closetag", {{"OPEN_BRACK", "SLASH", "STRING", "CLOSE_BRACK"}});
    addProductions(j, "members", {{"member"}, {"member", "members"}});
    addProduction(j, "member", {"element"});
    addProductions(j, "array", {{},{"elements"}});
    addProductions(j, "elements", {{"element"}, {"element", "elements"}});
    addProduction(j, "element", {"value"});

    file << std::setw(4) << j;
    file.close();
}

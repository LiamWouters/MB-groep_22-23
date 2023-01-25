#include <iostream>
#include "objects/LR1Parser.h"
#include "objects/CFG.h"
#include "objects/JsonGrammarGenerator.h"
#include "objects/EMLGrammarGenerator.h"
#include "objects/JsonSchema.h"
#include "objects/JsonTokenizer.h"
#include "objects/ParserComparison.h"
#include "../lib/nlohmann-json/json.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    JsonGrammarGenerator::generate();
    EMLGrammarGenerator::generateSimplified();
    /*
    for (int i = 0; i < 103; i++) {
        compareAllParsers("../tests/input/input-LR1_1.json");
        compareAllParsers("../tests/input/input-LR1_2.json");
        compareAllParsers("../tests/input/input-LR1_1.eml");
    }
    */
    // JsonSchema test
    /*
    std::string path = "../tests/input/input-json_schema1.json";
    std::string path2 = "../tests/input/input-json_tokenizer1.json";

    JsonSchema schem01(path);
    cout << schem01.validate(path2) << endl;
    */

    /*  LR1 PARSER TEST
    CFG* grammar = new CFG("../tests/input/input-LR1_1.json");
    LR1Parser* parser = new LR1Parser(*grammar, true);
    std::cout << parser->getPrintbuffer().str();
    bool accepted = parser->parse({"ident", "-", "ident"});
    //std::cout << parser->getPrintbuffer().str();
    if (accepted) { std::cout << "Parse string was accepted by the grammar! [LR1 parser]" << std::endl;}
    else { std::cout << "Parse string was NOT accepted by the grammar! [LR1 parser]" << std::endl; }
    */
    std::cout << "done!" << std::endl;
}
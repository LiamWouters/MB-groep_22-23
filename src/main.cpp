#include <iostream>
#include "objects/LR1Parser.h"
#include "objects/CFG.h"
#include "objects/JsonGrammarGenerator.h"

int main(int argc, char* argv[]) {
    //JsonGrammarGenerator::generate();

    /*  LR1 PARSER TEST
    CFG* grammar = new CFG("../tests/input/input-LR1_4.json");
    LR1Parser* parser = new LR1Parser(*grammar);
    bool accepted = parser->parse({"ident", "-", "ident"});
    if (accepted) { std::cout << "Parse string was accepted by the grammar! [LR1 parser]" << std::endl;}
    else { std::cout << "Parse string was NOT accepted by the grammar! [LR1 parser]" << std::endl; }
    */
    std::cout << "done!" << std::endl;
}
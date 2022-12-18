#include <iostream>
#include "LR1Parser.h"
#include "objects/CFG.h"
#include "objects/JsonGrammarGenerator.h"

int main(int argc, char* argv[]) {
    //JsonGrammarGenerator::generate();

    CFG* grammar = new CFG("../tests/input/input-LR1_1.json");
    LR1Parser* parser = new LR1Parser(*grammar);
    std::cout << "done!" << std::endl;
}
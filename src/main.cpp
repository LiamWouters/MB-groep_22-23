#include <iostream>
#include "objects/CFG.h"
#include "objects/JsonGrammarGenerator.h"

int main(int argc, char* argv[]) {
    std::cout << "Test!" << std::endl;
    JsonGrammarGenerator::generate();
}
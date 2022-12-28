#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/LR1Parser.h"
#include "../src/objects/FileConverter/Data.h"

TEST_SUITE("File Converter Tests") {
    TEST_CASE("[File Converter Tests] test0") {
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-FileConverter-json0.json");
        parser->parse(j.tokens);
        parser->printToJSON();
    }
}
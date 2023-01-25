//
// Created by axeld on 25/01/2023.
//

#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/EMLTokenizer.h"
#include "../src/objects/LL1Parser.h"

TEST_SUITE("LL1 Parser Tests"){
    /*
     * 2 things to test: check if LL1 parser can differentiate correct/error files, and in case of error
     * if it can find the index of the error.
     */
    TEST_CASE("[LL1 Parser Tests] test1"){
        CFG c("../res/json_grammar_simplified.json");
        LL1Parser l(c);
        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-LL1-true1.json");
        std::pair<bool, int> parse = l.accepts(j.tokens);
        CHECK_EQ(parse.first, true);
        CHECK_EQ(parse.second, -1);
    }
    TEST_CASE("[LL1 Parser Tests] test2"){
        CFG c("../res/json_grammar_simplified.json");
        LL1Parser l(c);
        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-LL1-false1.json");
        std::pair<bool, int> parse = l.accepts(j.tokens);
        CHECK_EQ(parse.first, false);
        CHECK_EQ(parse.second, 20);
    }
    TEST_CASE("[LL1 Parser Tests] test3"){
        CFG c("../res/eml_grammar_simplified.json");
        LL1Parser l(c);
        EMLTokenizer j;
        j.tokenizeSimplified("../tests/input/input-LL1-true2.eml");
        std::pair<bool, int> parse = l.accepts(j.tokens);
        CHECK_EQ(parse.first, true);
        CHECK_EQ(parse.second, -1);
    }
    TEST_CASE("[LL1 Parser Tests] test4"){
        CFG c("../res/eml_grammar_simplified.json");
        LL1Parser l(c);
        EMLTokenizer j;
        j.tokenizeSimplified("../tests/input/input-LL1-false2.eml");
        std::pair<bool, int> parse = l.accepts(j.tokens);
        CHECK_EQ(parse.first, false);
        CHECK_EQ(parse.second, 77);
    }
}
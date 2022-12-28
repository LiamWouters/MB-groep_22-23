//
// Created by michel on 14.12.22.
//

#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "../src/objects/JsonTokenizer.h"

TEST_SUITE("JsonTokenizerTests") {
    TEST_CASE("[JsonTokenizerTests] test0") {

        const std::ifstream expectedFile("../tests/expected/expected-json_tokenizer1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-json_tokenizer1.json");
        j.printTokens(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[JsonTokenizerTests] test1") {

        const std::ifstream expectedFile("../tests/expected/expected-json_tokenizer2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-json_tokenizer2.json");
        j.printTokens(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}
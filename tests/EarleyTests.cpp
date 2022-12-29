//
// Created by michel on 14.12.22.
//

#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/EarleyParser/EarleyParser.h"

TEST_SUITE("EarleyTests") {
    TEST_CASE("[EarleyTests] test0") {

        const std::ifstream expectedFile("../tests/expected/expected-earley1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        p.validateFile("../tests/input/input-earley1.json", Json);
        p.printChart(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[EarleyTests] test1") {

        const std::ifstream expectedFile("../tests/expected/expected-earley2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        p.validateFile("../tests/input/input-earley2.json", Json);
        p.printChart(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[EarleyTests] test2") {

        const std::ifstream expectedFile("../tests/expected/expected-earley3.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        p.validateFile("../tests/input/input-earley3.json", Json);
        p.printChart(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}
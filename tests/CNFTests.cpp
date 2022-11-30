#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "../src/objects/CFG.h"

TEST_SUITE("CNFTests") {
    TEST_CASE("[CNFTests] cnf0") {
        CFG cfg("../tests/input/input-cnf1.json");

        const std::ifstream expectedFile("../tests/expected/expected-cnf1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.toCNF(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[CNFTests] cnf1") {
        CFG cfg("../tests/input/input-cnf2.json");

        const std::ifstream expectedFile("../tests/expected/expected-cnf2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.toCNF(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}
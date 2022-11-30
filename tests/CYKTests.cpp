#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "../src/objects/CFG.h"

TEST_SUITE("CYKTests") {
    TEST_CASE("[CYKTests] cyk0") {
        CFG cfg("../tests/input/input-cyk1.json");

        const std::ifstream expectedFile("../tests/expected/expected-cyk1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.accepts("baaba", actual);
        cfg.accepts("abba", actual);

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[CYKTests] cyk1") {
        CFG cfg("../tests/input/input-cyk2.json");

        const std::ifstream expectedFile("../tests/expected/expected-cyk2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.accepts("aaabb", actual);
        cfg.accepts("aaabbb", actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}
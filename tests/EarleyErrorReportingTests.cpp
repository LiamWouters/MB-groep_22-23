
#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/EarleyParser/EarleyParser.h"

TEST_SUITE("EarleyErrorReportingTests") {
    TEST_CASE("[EarleyErrorReportingTests] test0") {

        const std::ifstream expectedFile("../tests/expected/expected-earley_error_reporting1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        if (!p.validateFile("../tests/input/input-earley_error_reporting1.json", Json)){
            p.printErrorReport(Json, "../tests/input/input-earley_error_reporting1.json", actual);
        }

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[EarleyErrorReportingTests] test1") {

        const std::ifstream expectedFile("../tests/expected/expected-earley_error_reporting2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        if (!p.validateFile("../tests/input/input-earley_error_reporting2.json", Json)){
            p.printErrorReport(Json, "../tests/input/input-earley_error_reporting2.json", actual);
        }

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[EarleyErrorReportingTests] test2") {

        const std::ifstream expectedFile("../tests/expected/expected-earley_error_reporting3.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        if (!p.validateFile("../tests/input/input-earley_error_reporting3.json", Json)){
            p.printErrorReport(Json, "../tests/input/input-earley_error_reporting3.json", actual);
        }

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[EarleyErrorReportingTests] test3") {

        const std::ifstream expectedFile("../tests/expected/expected-earley_error_reporting4.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        EarleyParser p("../res/json_grammar_simplified.json");
        if (!p.validateFile("../tests/input/input-earley_error_reporting4.json", Json)){
            p.printErrorReport(Json, "../tests/input/input-earley_error_reporting4.json", actual);
        }

        CHECK_EQ(expected.str(), actual.str());
    }
}
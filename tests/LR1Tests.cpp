#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "../src/objects/LR1Parser.h"

TEST_SUITE("LR1 Parser Tests") {
    TEST_CASE("[LR1 Parser Tests] test0") {
        // non-json/eml test
        CFG* grammar = new CFG("../tests/input/input-LR1_1.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        const std::ifstream expectedFile("../tests/expected/expected-LR1_1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        CHECK_EQ(parser->getPrintbuffer().str(), expected.str());

        const std::ifstream expectedParseFile("../tests/expected/expected-LR1_1-parse.txt");
        expected.str(std::string());
        expected << expectedParseFile.rdbuf();

        parser->parse({"ident", "-", "ident"});

        CHECK_EQ(parser->getPrintbuffer().str(), expected.str());
    }
    TEST_CASE("[LR1 Parser Tests] test1") {
        // non-json/eml test
        CFG* grammar = new CFG("../tests/input/input-LR1_2.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        const std::ifstream expectedFile("../tests/expected/expected-LR1_2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        CHECK_EQ(parser->getPrintbuffer().str(), expected.str());

        const std::ifstream expectedParseFile("../tests/expected/expected-LR1_2-parse.txt");
        expected.str(std::string());
        expected << expectedParseFile.rdbuf();

        CHECK_FALSE(parser->parse({"ident", "-", "ident"})); // not in the grammar
        parser->parse({"c", "d", "c", "c", "d"});
        /*
        std::ofstream file ("../tests/expected/testPrint.txt");
        file << parser->getPrintbuffer().str();
        file.close();
        */
        CHECK_EQ(parser->getPrintbuffer().str(), expected.str());
    }
    TEST_CASE("[LR1 Parser Tests] test_json") {
        // json TEST
        /*
        CFG* grammar = new CFG("../res/json_grammar.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        const std::ifstream expectedFile("../tests/expected/expected-LR1_1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::ofstream file("../tests/expected/TEST.txt");
        file << parser->getPrintbuffer().str();
        file.close();
        */
        //CHECK_EQ(parser->getPrintbuffer().str(), expected.str());
    }
}
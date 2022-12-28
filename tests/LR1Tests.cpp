#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/LR1Parser.h"

TEST_SUITE("LR1 Parser Tests") {
    TEST_CASE("[LR1 Parser Tests] test0") {
        // non-json/eml test
        CFG* grammar = new CFG("../tests/input/input-LR1_1.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        //parser->saveParser("input-LR1_1-SAVEDPARSER");

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
    TEST_CASE("[LR1 Parser Tests] test0-SAVEDPARSER") {
        // non-json/eml test
        // tests if parsers save/load correctly
        CFG* grammar = new CFG("../tests/input/input-LR1_1.json");
        LR1Parser* parser = new LR1Parser("../tests/input/input-LR1_1-SAVEDPARSER.json", *grammar, true);

        const std::ifstream expectedFile("../tests/expected/expected-LR1_1-SAVEDPARSER.txt");
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
    TEST_CASE("[LR1 Parser Tests] test_jsonParseTable") {
        // json parse table TEST
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);
        //parser->saveParser("saved_JSON_LR1ParseTable");

        const std::ifstream expectedFile("../tests/expected/expected-LR1-jsonSimplifiedparseTable.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        CHECK_EQ(parser->getPrintbuffer().str(), expected.str());
    }
    TEST_CASE("[LR1 Parser Tests] test_json-parse0") {
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        // parse basic json file (a CFG file)
        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-LR1_1.json"); // CFG input file, is valid json, it should parse
        CHECK(parser->parse(j.tokens));

        // parse bigger json file (json grammar simplified)
        JsonTokenizer j1;
        j1.tokenizeSimplified("../res/json_grammar_simplified.json"); // bigger CFG input file, should parse
        CHECK(parser->parse(j1.tokens));

        // parse even bigger json file
        JsonTokenizer j2;
        j2.tokenizeSimplified("../tests/input/input-LR1-large-file.json"); // large file found online, source: look at README, should parse
        CHECK(parser->parse(j2.tokens));
    }
    TEST_CASE("[LR1 Parser Tests] test_FALSEjson-parse") {
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        // missing "
        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-LR1_falseJSON-0.json");
        CHECK_FALSE(parser->parse(j.tokens));

        // added , at last member
        JsonTokenizer j1;
        j1.tokenizeSimplified("../tests/input/input-LR1_falseJSON-1.json");
        CHECK_FALSE(parser->parse(j1.tokens));

        // missing ]
        JsonTokenizer j2;
        j2.tokenizeSimplified("../tests/input/input-LR1_falseJSON-2.json");
        CHECK_FALSE(parser->parse(j2.tokens));

        // missing :
        JsonTokenizer j3;
        j3.tokenizeSimplified("../tests/input/input-LR1_falseJSON-3.json");
        CHECK_FALSE(parser->parse(j3.tokens));

        // missing ,
        JsonTokenizer j4;
        j4.tokenizeSimplified("../tests/input/input-LR1_falseJSON-4.json");
        CHECK_FALSE(parser->parse(j4.tokens));

        // missing }
        JsonTokenizer j5;
        j5.tokenizeSimplified("../tests/input/input-LR1_falseJSON-5.json");
        CHECK_FALSE(parser->parse(j5.tokens));

        // missing initial element
        JsonTokenizer j6;
        j6.tokenizeSimplified("../tests/input/input-LR1_falseJSON-6.json");
        CHECK_FALSE(parser->parse(j6.tokens));

        // extra ,
        JsonTokenizer j7;
        j7.tokenizeSimplified("../tests/input/input-LR1_falseJSON-7.json");
        CHECK_FALSE(parser->parse(j7.tokens));
    }
}
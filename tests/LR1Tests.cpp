#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/EMLTokenizer.h"
#include "../src/objects/EMLGrammarGenerator.h"
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

        delete parser;
        delete grammar;
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

        delete parser;
        delete grammar;
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

        delete parser;
        delete grammar;
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

        delete parser;
        delete grammar;
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

        delete parser;
        delete grammar;
    }

    TEST_CASE("[LR1 Parser Tests] test_emlParseTable") {
        // json parse table TEST
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        const std::ifstream expectedFile("../tests/expected/expected-LR1-emlSimplifiedparseTable.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        CHECK_EQ(parser->getPrintbuffer().str(), expected.str());

        delete parser;
        delete grammar;
    }
    TEST_CASE("[LR1 Parser Tests] test_eml-parse0") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        // parse basic json file (a CFG file)
        EMLTokenizer e;
        e.tokenizeSimplified("../tests/input/input-LR1_1.eml"); // CFG input file, is valid eml, it should parse
        CHECK(parser->parse(e.tokens));

        // parse bigger eml file (eml grammar simplified)
        EMLTokenizer e1;
        e1.tokenizeSimplified("../tests/expected/expected_EMLCONVERTED_json_grammar_simplified.eml"); // bigger CFG input file, should parse
        CHECK(parser->parse(e1.tokens));

        // parse even bigger json file -> convert to EML, parse that file (to validate), convert back to JSON to check if nothing is lost
        CFG* jsongrammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* jsonparser = new LR1Parser(*jsongrammar, true);
        JsonTokenizer j2;
        j2.tokenizeSimplified("../tests/input/input-LR1-large-file.json"); // source: look at README, should parse
        CHECK(jsonparser->parse(j2.tokens));
        CHECK(jsonparser->printToEML());

        EMLTokenizer e2;
        e2.tokenizeSimplified("../res/EML-conversion-output.eml");
        CHECK(parser->parse(e2.tokens));
        parser->printToJSON();

        // this is partially a File Conversion Test
        JsonTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");

        if (jPrinted.tokens.size() == j2.tokens.size()) {
            for (int i = 0; i < j2.tokens.size(); i++) {
                CHECK_EQ(jPrinted.tokens[i].type, j2.tokens[i].type);
                CHECK_EQ(jPrinted.tokens[i].content, j2.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        delete parser;
        delete grammar;
    }
    TEST_CASE("[LR1 Parser Tests] test_FALSEeml-parse") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        // missing "
        EMLTokenizer e;
        e.tokenizeSimplified("../tests/input/input-LR1_falseEML-0.eml");
        CHECK_FALSE(parser->parse(e.tokens));

        // added , at last element in array
        EMLTokenizer e1;
        e1.tokenizeSimplified("../tests/input/input-LR1_falseEML-1.eml");
        CHECK_FALSE(parser->parse(e1.tokens));

        // missing array close tag
        EMLTokenizer e2;
        e2.tokenizeSimplified("../tests/input/input-LR1_falseEML-2.eml");
        CHECK_FALSE(parser->parse(e2.tokens));

        // incomplete tag
        EMLTokenizer e3;
        e3.tokenizeSimplified("../tests/input/input-LR1_falseEML-3.eml");
        CHECK_FALSE(parser->parse(e3.tokens));

        // missing ,
        EMLTokenizer e4;
        e4.tokenizeSimplified("../tests/input/input-LR1_falseEML-4.eml");
        CHECK_FALSE(parser->parse(e4.tokens));

        // missing root close
        EMLTokenizer e5;
        e5.tokenizeSimplified("../tests/input/input-LR1_falseEML-5.eml");
        CHECK_FALSE(parser->parse(e5.tokens));

        // missing root open
        EMLTokenizer e6;
        e6.tokenizeSimplified("../tests/input/input-LR1_falseEML-6.eml");
        CHECK_FALSE(parser->parse(e6.tokens));

        // extra ,
        EMLTokenizer e7;
        e7.tokenizeSimplified("../tests/input/input-LR1_falseEML-7.eml");
        CHECK_FALSE(parser->parse(e7.tokens));

        delete parser;
        delete grammar;
    }
}
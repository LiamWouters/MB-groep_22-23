#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/LR1Parser.h"
#include "../src/objects/FileConverter/Data.h"

TEST_SUITE("File Converter Tests") {
    /*
     * pre conditions of file converter: file must be valid (otherwise not converted, we know if a file is valid if the parser parses correctly)
     */
    TEST_CASE("[File Converter Tests] JSONSTART_test0") {
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-FileConverter-json0.json");
        parser->parse(j.tokens);

        parser->printToJSON();

        // tokenize printed file to see if it is the same as the
        JsonTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");

        if (j.tokens.size() == jPrinted.tokens.size()) {
            for (int i = 0; i < j.tokens.size(); i++) {
                CHECK_EQ(j.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(j.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }
        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] JSONSTART_test1-minimal") {
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-FileConverter-json1.json");
        parser->parse(j.tokens);

        parser->printToJSON();

        // tokenize printed file to see if it is the same as the
        JsonTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");

        if (j.tokens.size() == jPrinted.tokens.size()) {
            for (int i = 0; i < j.tokens.size(); i++) {
                CHECK_EQ(j.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(j.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        //// second minimal file
        JsonTokenizer j2;
        j2.tokenizeSimplified("../tests/input/input-FileConverter-json2.json");
        parser->parse(j2.tokens);

        parser->printToJSON();

        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");

        if (j2.tokens.size() == jPrinted.tokens.size()) {
            for (int i = 0; i < j2.tokens.size(); i++) {
                CHECK_EQ(j2.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(j2.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }
        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] JSONSTART_test2-big") {
        CFG* grammar = new CFG("../res/json_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        JsonTokenizer j;
        j.tokenizeSimplified("../res/json_grammar_simplified.json");
        parser->parse(j.tokens);

        parser->printToJSON();

        // tokenize printed file to see if it is the same as the
        JsonTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");

        if (j.tokens.size() == jPrinted.tokens.size()) {
            for (int i = 0; i < j.tokens.size(); i++) {
                CHECK_EQ(j.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(j.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }
        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] EMLSTART_test0") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        CHECK(false); // EML not implemented yet
        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] EMLSTART_test1-minimal") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        CHECK(false); // EML not implemented yet
        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] EMLSTART_test2-big") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        CHECK(false); // EML not implemented yet
        delete parser;
        delete grammar;
    }
}
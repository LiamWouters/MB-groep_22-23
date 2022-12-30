#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/JsonTokenizer.h"
#include "../src/objects/EMLTokenizer.h"
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

        CHECK(parser->printToJSON());

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

        /// GO FROM JSON -> EML
        parser->printToEML();
        // tokenize the generated file
        EMLTokenizer ePrinted;
        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");
        // get expected json file
        EMLTokenizer eExpected;
        eExpected.tokenizeSimplified("../tests/input/input-FileConverter-eml0.eml");

        if (eExpected.tokens.size() == ePrinted.tokens.size()) {
            for (int i = 0; i < ePrinted.tokens.size(); i++) {
                CHECK_EQ(eExpected.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(eExpected.tokens[i].content, ePrinted.tokens[i].content);
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

        //// FIRST MINIMAL FILE
        JsonTokenizer j;
        j.tokenizeSimplified("../tests/input/input-FileConverter-json1.json");
        parser->parse(j.tokens);

        CHECK(parser->printToJSON());

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

        // GO FROM JSON -> EML
        CHECK(parser->printToEML());
        EMLTokenizer ePrinted;
        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");
        EMLTokenizer eExpected;
        eExpected.tokenizeSimplified("../tests/input/input-FileConverter-eml1.eml");

        if (eExpected.tokens.size() == ePrinted.tokens.size()) {
            for (int i = 0; i < eExpected.tokens.size(); i++) {
                CHECK_EQ(eExpected.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(eExpected.tokens[i].content, ePrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        //// second minimal file
        JsonTokenizer j2;
        j2.tokenizeSimplified("../tests/input/input-FileConverter-json2.json");
        parser->parse(j2.tokens);

        CHECK(parser->printToJSON());

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

        // GO FROM JSON -> EML
        CHECK(parser->printToEML());
        EMLTokenizer ePrinted2;
        ePrinted2.tokenizeSimplified("../res/EML-conversion-output.eml");
        EMLTokenizer eExpected2;
        eExpected2.tokenizeSimplified("../tests/input/input-FileConverter-eml2.eml");

        if (eExpected2.tokens.size() == ePrinted2.tokens.size()) {
            for (int i = 0; i < eExpected2.tokens.size(); i++) {
                CHECK_EQ(eExpected2.tokens[i].type, ePrinted2.tokens[i].type);
                CHECK_EQ(eExpected2.tokens[i].content, ePrinted2.tokens[i].content);
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

        CHECK(parser->printToJSON());

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

        /// JSON -> EML
        CHECK(parser->printToEML());
        EMLTokenizer ePrinted;
        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");
        EMLTokenizer eExpected;
        eExpected.tokenizeSimplified("../tests/expected/expected_EMLCONVERTED_json_grammar_simplified.eml");

        if (ePrinted.tokens.size() == eExpected.tokens.size()) {
            for (int i = 0; i < ePrinted.tokens.size(); i++) {
                CHECK_EQ(eExpected.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(eExpected.tokens[i].content, ePrinted.tokens[i].content);
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

        EMLTokenizer e;
        e.tokenizeSimplified("../tests/input/input-FileConverter-eml0.eml");
        parser->parse(e.tokens);

        CHECK(parser->printToEML());

        // tokenize printed file to see if it is the same as the
        EMLTokenizer ePrinted;
        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");

        if (e.tokens.size() == ePrinted.tokens.size()) {
            for (int i = 0; i < e.tokens.size(); i++) {
                CHECK_EQ(e.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(e.tokens[i].content, ePrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        /// GO FROM EML -> JSON
        parser->printToJSON();
        // tokenize the generated file
        JsonTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");
        // get expected json file
        JsonTokenizer jExpected;
        jExpected.tokenizeSimplified("../tests/input/input-FileConverter-json0.json");

        if (jExpected.tokens.size() == jPrinted.tokens.size()) {
            for (int i = 0; i < jPrinted.tokens.size(); i++) {
                CHECK_EQ(jExpected.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(jExpected.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] EMLSTART_test1-minimal") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        //// FIRST MINIMAL FILE
        EMLTokenizer e;
        e.tokenizeSimplified("../tests/input/input-FileConverter-eml1.eml");
        parser->parse(e.tokens);

        CHECK(parser->printToEML());

        // tokenize printed file to see if it is the same as the
        EMLTokenizer ePrinted;
        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");

        if (e.tokens.size() == ePrinted.tokens.size()) {
            for (int i = 0; i < e.tokens.size(); i++) {
                CHECK_EQ(e.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(e.tokens[i].content, ePrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        // GO FROM EML -> JSON
        CHECK(parser->printToJSON());
        EMLTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");
        EMLTokenizer jExpected;
        jExpected.tokenizeSimplified("../tests/input/input-FileConverter-json1.json");

        if (jExpected.tokens.size() == jPrinted.tokens.size()) {
            for (int i = 0; i < jExpected.tokens.size(); i++) {
                CHECK_EQ(jExpected.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(jExpected.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        //// second minimal file
        EMLTokenizer e2;
        e2.tokenizeSimplified("../tests/input/input-FileConverter-eml2.eml");
        parser->parse(e2.tokens);

        CHECK(parser->printToEML());

        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");

        if (e2.tokens.size() == ePrinted.tokens.size()) {
            for (int i = 0; i < e2.tokens.size(); i++) {
                CHECK_EQ(e2.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(e2.tokens[i].content, ePrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        // GO FROM EML -> JSON
        CHECK(parser->printToJSON());
        EMLTokenizer jPrinted2;
        jPrinted2.tokenizeSimplified("../res/JSON-conversion-output.json");
        EMLTokenizer jExpected2;
        jExpected2.tokenizeSimplified("../tests/input/input-FileConverter-json2.json");

        if (jExpected2.tokens.size() == jPrinted2.tokens.size()) {
            for (int i = 0; i < jExpected2.tokens.size(); i++) {
                CHECK_EQ(jExpected2.tokens[i].type, jPrinted2.tokens[i].type);
                CHECK_EQ(jExpected2.tokens[i].content, jPrinted2.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        delete parser;
        delete grammar;
    }
    TEST_CASE("[File Converter Tests] EMLSTART_test2-big") {
        CFG* grammar = new CFG("../res/eml_grammar_simplified.json");
        LR1Parser* parser = new LR1Parser(*grammar, true);

        EMLTokenizer e;
        e.tokenizeSimplified("../tests/expected/expected_EMLCONVERTED_json_grammar_simplified.eml");
        parser->parse(e.tokens);

        CHECK(parser->printToEML());

        // tokenize printed file to see if it is the same as the
        EMLTokenizer ePrinted;
        ePrinted.tokenizeSimplified("../res/EML-conversion-output.eml");

        if (e.tokens.size() == ePrinted.tokens.size()) {
            for (int i = 0; i < e.tokens.size(); i++) {
                CHECK_EQ(e.tokens[i].type, ePrinted.tokens[i].type);
                CHECK_EQ(e.tokens[i].content, ePrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        /// EML -> JSON
        CHECK(parser->printToJSON());
        JsonTokenizer jPrinted;
        jPrinted.tokenizeSimplified("../res/JSON-conversion-output.json");
        JsonTokenizer jExpected;
        jExpected.tokenizeSimplified("../res/json_grammar_simplified.json");

        if (jPrinted.tokens.size() == jExpected.tokens.size()) {
            for (int i = 0; i < jPrinted.tokens.size(); i++) {
                CHECK_EQ(jExpected.tokens[i].type, jPrinted.tokens[i].type);
                CHECK_EQ(jExpected.tokens[i].content, jPrinted.tokens[i].content);
            }
        }
        else {
            CHECK(false);
        }

        delete parser;
        delete grammar;
    }
}
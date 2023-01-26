#include "../lib/doctest/doctest.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/objects/ParserComparison.h"

TEST_SUITE("Parser Comparison Tests") {
    TEST_CASE("[Parser Comparison Tests] testJSONcomp0") {
        std::string testOutputFile = "../tests/expected/test-output-htmlCompTable.html";

        std::ofstream file(testOutputFile, std::ofstream::trunc); // clear file
        file.close();

        compareAllParsers("../tests/input/input-LR1_1.json", 1, testOutputFile);

        std::ifstream oFile(testOutputFile);
        std::stringstream content;
        content << oFile.rdbuf();
        std::string contentString = content.str();
        oFile.close();
        CHECK_NE(contentString.find("<!DOCTYPE html>\n<html>\n<head> <h1>Parser Comparison Table: </h1> </head>\n<body>\n<p1>(values in microseconds)</p1>\n<table border=\"5\">\n<tr>\n<th>File name</th>\n<th>Earley parser</th>\n<th>LR1 parser</th>\n<th>LL1 parser</th>\n<th>#Scans</th>\n</tr>"), contentString.npos);
        CHECK_NE(contentString.find("<td>input-LR1_1.json</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>1</td>"), std::string::npos);

        compareAllParsers("../tests/input/input-LR1_1.json", 1, testOutputFile);

        oFile.open(testOutputFile);
        content.str(""); // clear stringstream
        content << oFile.rdbuf();
        contentString = content.str();
        oFile.close();
        CHECK_EQ(contentString.find("<td>1</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>2</td>"), std::string::npos);
    }
    TEST_CASE("[Parser Comparison Tests] testJSONcomp1") {
        std::string testOutputFile = "../tests/expected/test-output-htmlCompTable.html";

        std::ofstream file(testOutputFile, std::ofstream::trunc); // clear file
        file.close();

        compareAllParsers("../tests/input/input-LR1_1.json", 3, testOutputFile);

        std::ifstream oFile(testOutputFile);
        std::stringstream content;
        content << oFile.rdbuf();
        std::string contentString = content.str();
        oFile.close();
        CHECK_NE(contentString.find("<td>input-LR1_1.json</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>3</td>"), std::string::npos);
        CHECK_EQ(contentString.find("<td>input-LR1_2.json</td>"), std::string::npos);
        CHECK_EQ(contentString.find("<td>1</td>"), std::string::npos);

        compareAllParsers("../tests/input/input-LR1_2.json", 1, testOutputFile);

        oFile.open(testOutputFile);
        content.str("");
        content << oFile.rdbuf();
        contentString = content.str();
        oFile.close();
        CHECK_NE(contentString.find("<td>input-LR1_1.json</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>3</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>input-LR1_2.json</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>1</td>"), std::string::npos);
    }
    TEST_CASE("[Parser Comparison Tests] testEMLcomp0") {
        std::string testOutputFile = "../tests/expected/test-output-htmlCompTable.html";

        std::ofstream file(testOutputFile, std::ofstream::trunc); // clear file
        file.close();

        compareAllParsers("../tests/input/input-LR1_1.eml", 1, testOutputFile);

        std::ifstream oFile(testOutputFile);
        std::stringstream content;
        content << oFile.rdbuf();
        std::string contentString = content.str();
        oFile.close();
        CHECK_NE(contentString.find("<td>input-LR1_1.eml</td>"), std::string::npos);
        CHECK_NE(contentString.find("<td>1</td>"), std::string::npos);
    }
}
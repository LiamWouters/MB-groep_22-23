#include <fstream>
#include <chrono>
#include <sstream>
#include "LR1Parser.h"
#include "EarleyParser/EarleyParser.h"
#include "LL1Parser.h"
#include "JsonTokenizer.h"
#include "EMLTokenizer.h"
#include "ParserComparison.h"
using std::string;

void compareAllParsers(std::string inputFilePath, int amount) {
    // get file name
    string fileName = inputFilePath;
    fileName.erase(0, fileName.rfind("/")+1);

    // get file type
    if (fileName.find('.') == fileName.npos) {std::cout << "compareAllParsers ERROR: file error" << std::endl; return;}
    string fileType = fileName.substr(fileName.find('.'), fileName.npos);

    // create parsers
    JsonTokenizer jTokenizer;
    EMLTokenizer eTokenizer;
    LR1Parser* lrparser = nullptr;
    LL1Parser* llparser = nullptr;
    EarleyParser* eparser = nullptr;
    if (fileType == ".json") {
        jTokenizer.tokenizeSimplified(inputFilePath);
        CFG grammar("../res/json_grammar_simplified.json");
        lrparser = new LR1Parser(grammar, false);
        llparser = new LL1Parser(grammar);
        eparser = new EarleyParser(grammar);
    } else if (fileType == ".eml") {
        eTokenizer.tokenizeSimplified(inputFilePath);
        CFG grammar("../res/eml_grammar_simplified.json");
        lrparser = new LR1Parser(grammar, true);
        llparser = new LL1Parser(grammar);
        eparser = new EarleyParser(grammar);
    } else {std::cout << "compareAllParsers ERROR: invalid file type" << std::endl; return;}

    // gather parser time data
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    int lrTime{0}, llTime{0}, earleyTime{0};
    bool parsedCorrectly;

    for (int i = 0; i<amount; i++) {
        if (fileType == ".json") {
            start = std::chrono::high_resolution_clock::now();
            parsedCorrectly = lrparser->parse(jTokenizer.tokens);
            stop = std::chrono::high_resolution_clock::now();
            lrTime += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            if (!parsedCorrectly) {
                std::cout << "PARSER COMPARISON ERROR: input file must be parseable" << std::endl;
                return;
            }

            start = std::chrono::high_resolution_clock::now();
            parsedCorrectly = llparser->accepts(jTokenizer.tokens).first;
            stop = std::chrono::high_resolution_clock::now();
            llTime += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            if (!parsedCorrectly) {
                std::cout << "PARSER COMPARISON ERROR: input file must be parseable" << std::endl;
                return;
            }

            start = std::chrono::high_resolution_clock::now();
            parsedCorrectly = eparser->validate(jTokenizer.tokens);
            stop = std::chrono::high_resolution_clock::now();
            earleyTime += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            if (!parsedCorrectly) {
                std::cout << "PARSER COMPARISON ERROR: input file must be parseable" << std::endl;
                return;
            }
        } else {
            start = std::chrono::high_resolution_clock::now();
            parsedCorrectly = lrparser->parse(eTokenizer.tokens);
            stop = std::chrono::high_resolution_clock::now();
            lrTime += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            if (!parsedCorrectly) {
                std::cout << "PARSER COMPARISON ERROR: input file must be parseable" << std::endl;
                return;
            }

            start = std::chrono::high_resolution_clock::now();
            parsedCorrectly = llparser->accepts(eTokenizer.tokens).first;
            stop = std::chrono::high_resolution_clock::now();
            llTime += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            if (!parsedCorrectly) {
                std::cout << "PARSER COMPARISON ERROR: input file must be parseable" << std::endl;
                return;
            }

            start = std::chrono::high_resolution_clock::now();
            parsedCorrectly = eparser->validate(eTokenizer.tokens);
            stop = std::chrono::high_resolution_clock::now();
            earleyTime += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            if (!parsedCorrectly) {
                std::cout << "PARSER COMPARISON ERROR: input file must be parseable" << std::endl;
                return;
            }
        }
    }

    // check if HTML table file exists
    std::ifstream inpHtmlTable("../res/HTMLcomparisonTable.html");
    if (!inpHtmlTable.good()) {
        // doesn't exist
        std::ofstream htmlTable("../res/HTMLcomparisonTable.html");
        htmlTable << "<!DOCTYPE html>\n"
                     "<html>\n"
                     "<head> <h1>Parser Comparison Table: </h1> </head>\n"
                     "<body>\n"
                     "<p1>(values in microseconds)</p1>\n"
                     "<table border=\"5\">\n"
                     "<tr>\n"
                     "<th>File name</th>\n"
                     "<th>Earley parser</th>\n"
                     "<th>LR1 parser</th>\n"
                     "<th>LL1 parser</th>\n"
                     "<th>#Scans</th>\n"
                     "</tr>\n"
                     "</table>\n"
                     "</body>\n"
                     "</html>";
        htmlTable.close();
    }
    inpHtmlTable.close();

    // put time data in table
    inpHtmlTable.open("../res/HTMLcomparisonTable.html");
    string htmlLine;
    std::ostringstream newFile;
    bool added = false;
    while (getline(inpHtmlTable, htmlLine)) {
        if (!(htmlLine.find("</table>") == htmlLine.npos) && !added) {
            // create new table entry
            newFile << "<tr>\n<td>" << fileName << "</td>\n<td>" << earleyTime/amount << "</td>\n<td>" << lrTime/amount << "</td>\n<td>" << llTime/amount << "</td>\n<td>" << amount << "</td>\n</tr>\n";
        }
        // replace existing
        newFile << htmlLine << "\n";
        if (htmlLine.find("<tr>") == htmlLine.npos) { continue; }
        getline(inpHtmlTable, htmlLine);
        newFile << htmlLine << "\n";
        if (htmlLine.find("<td>") == htmlLine.npos) { continue; }
        if (htmlLine.find(fileName) == htmlLine.npos) { continue; }
        // existing file name
        getline(inpHtmlTable, htmlLine); // original earley time
        if (htmlLine.find("<td>") == htmlLine.npos) { continue; }
        int oge = std::stoi(htmlLine.substr(htmlLine.find("<td>")+4, htmlLine.find("</td>")-4));
        getline(inpHtmlTable, htmlLine); // original lr1 parser time
        if (htmlLine.find("<td>") == htmlLine.npos) { continue; }
        int oglr = std::stoi(htmlLine.substr(htmlLine.find("<td>")+4, htmlLine.find("</td>")-4));
        getline(inpHtmlTable, htmlLine); // original ll1 parser time
        if (htmlLine.find("<td>") == htmlLine.npos) { continue; }
        int ogll = std::stoi(htmlLine.substr(htmlLine.find("<td>")+4, htmlLine.find("</td>")-4));
        getline(inpHtmlTable, htmlLine); // get #scans
        if (htmlLine.find("<td>") == htmlLine.npos) { continue; }
        int scans = std::stoi(htmlLine.substr(htmlLine.find("<td>")+4, htmlLine.find("</td>")-4));
        newFile << "<td>" << ((oge*scans)+earleyTime)/(scans+amount) << "</td>\n";
        newFile << "<td>" << ((oglr*scans)+lrTime)/(scans+amount) << "</td>\n";
        newFile << "<td>" << ((ogll*scans)+llTime)/(scans+amount) << "</td>\n";
        newFile << "<td>" << scans+1 << "</td>\n";

        added = true;
    }
    inpHtmlTable.close();
    std::ofstream newOutputFile("../res/HTMLcomparisonTable.html");
    newOutputFile << newFile.str();
    newOutputFile.close();
}

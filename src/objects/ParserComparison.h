#ifndef MB_GROEP_22_23_PARSERCOMPARISON_H
#define MB_GROEP_22_23_PARSERCOMPARISON_H

#include <iostream>

/*
 * Compare the performance of all parsers for the given file
 * This function will add to the html table file containing performance data
 * (or create the file if it doesn't exist yet)
 * return value = error if failed, "" if succeeded
 */
std::string compareAllParsers(std::string inputFilePath, int amount = 1, std::string outputFilePath = "../res/HTMLcomparisonTable.html");

#endif //MB_GROEP_22_23_PARSERCOMPARISON_H

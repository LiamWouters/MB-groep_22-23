#ifndef MB_GROEP_22_23_PARSERCOMPARISON_H
#define MB_GROEP_22_23_PARSERCOMPARISON_H

#include <iostream>

/*
 * Compare the performance of all parsers for the given file
 * This function will add to the html table file containing performance data
 * (or create the file if it doesn't exist yet)
 */
void compareAllParsers(std::string inputFilePath, int amount = 1);

#endif //MB_GROEP_22_23_PARSERCOMPARISON_H

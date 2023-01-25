//
// Created by axeld on 11/01/2023.
//

#ifndef MB_GROEP_22_23_COMMANDLINEINTERFACE_H
#define MB_GROEP_22_23_COMMANDLINEINTERFACE_H
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "LR1Parser.h"
#include "LL1Parser.h"
#include "EarleyParser/EarleyParser.h"
#include "CFG.h"

// Keeps track of how many output files have been made so far, so they're numbered differently
static int outputs = 1;

class CommandLineInterface {
public:
    enum state{start, selectFile, parserSelect, schemaSelect, yesNo, more, exiting};
    enum parser{null, ll, lr, earley};
    enum fileType{none, json, eml};
    static void simulate();
private:
    static void getSchemas();
    static void help(const state&);
    static bool validCommand(const state&, const std::string&);
    static void parse(const parser&, const fileType&, const std::string&, const std::string&, bool&, bool&, bool&);
};

#endif //MB_GROEP_22_23_COMMANDLINEINTERFACE_H
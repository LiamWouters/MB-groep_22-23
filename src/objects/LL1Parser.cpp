//
// Created by axeld on 18/12/2022.
//

#include "LL1Parser.h"

LL1Parser::LL1Parser(const CFG &c){
    grammar = c;
}

bool LL1Parser::accepts(const std::string &input){
    /*
     * The LL Parser works top-down as in from start symbol to string.
     * It uses 2 techniques: expect (a production), and match.
     *
     * Terminals at the front of the current string must be matched with the input string,
     * if they match, remove the terminals from both the input and current string.
     *
     * If the current string has a variable in front, it means we expect to execute a
     * certain production, which we can look up in the "expect" table.
     * This table is granted via a public member function in the class CFG.
     *
     * At any point it might be possible that no match is possible, or there is no valid
     * production to expect (= <ERR>), in that case the input string is not accepted.
     *
     * ==================================================================================
     *
     * Step 1: fetch the "expect" table, get the starting state and copy the input.
     */
    auto expect = grammar.lltable();
    std::vector<std::string> terminals = grammar.getTerminals();
    std::string current = grammar.getStartState();
    std::string inputCopy = input;
    // This variable was necessary to prevent some errors.
    std::vector<std::string> error = {"<ERR>"};
    // Step 2: Let the parsing begin!
    // As long as not both the input and current string are empty, operations must be done.
    while(!(inputCopy.empty() && current.empty())){
        // Fetch the first character (= "<EOS>" if string empty).
        std::string currentChar(1, inputCopy[0]);
        if(inputCopy.empty()){currentChar = "<EOS>";}
        // At this point the first character of the current string is always a variable,
        // which means we expect a certain production.
        if(expect[std::make_pair(current, currentChar)] == error){return false;}
        // Replace the symbol with the expected production rule.
        std::string replace;
        for(auto &i: expect[std::make_pair(current, currentChar)]){replace += i;}
        if(current.size() > 1){replace += current.substr(1, current.size()-1);}
        current = replace;
        // If the first character of the current string is a terminal, execute a match.
        std::string first(1, current[0]);
        if(std::find(terminals.begin(), terminals.end(), first) != terminals.end()){
            if(!match(inputCopy, current)){return false;}
        }
        if(current.empty() && !inputCopy.empty()){return false;}
    }
    // If both are empty then the parsing was a success.
    return true;
}

bool LL1Parser::match(std::string& input, std::string &current){
    int matches = 0;
    while(input[0] == current[0]){
        input = input.substr(1, input.size()-1);
        current = current.substr(1, current.size()-1);
        matches += 1;
        if(input.empty() && current.empty()){return true;}
    }
    return matches > 0;
}


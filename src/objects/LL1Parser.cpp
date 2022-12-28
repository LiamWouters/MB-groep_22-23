//
// Created by axeld on 18/12/2022.
//

#include "LL1Parser.h"

LL1Parser::LL1Parser(const CFG &c) : grammar(c) {}

bool LL1Parser::accepts(const std::vector<token> &input){
    /*
     * The LL Parser works top-down as in from start symbol to string.
     * It uses 2 techniques: expect (a production), and match (terminals).
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
    token start(grammar.getStartState(), 1, 0);
    std::vector<token> current = {start};
    std::vector<token> inputCopy = input;
    // These variables were necessary to prevent some errors.
    std::vector<std::string> error = {"<ERR>"};
    token eos("<EOS>", 0, 0);
    // Step 2: Let the parsing begin!
    // As long as not both the input and current string are empty, operations must be done.
    while(!(inputCopy.empty() && current.empty())){
        // Fetch the first character (= "<EOS>" if string empty).
        token currentChar = inputCopy[0];
        if(inputCopy.empty()){currentChar = eos;}
        // At this point the first character of the current string is always a variable,
        // which means we expect a certain production.
        token currentSymbol = current[0];
        if(expect[std::make_pair(currentSymbol.content, currentChar.content)] == error){return false;}
        // Replace the symbol with the expected production rule.
        std::vector<token> replace;
        for(auto &i: expect[std::make_pair(currentSymbol.content, currentChar.content)]){
            token t(i, replace.end()-replace.begin(), 0);
            replace.emplace_back(t);
        }
        if(current.size() > 1){
            for(int i = 1; i < current.size(); i++){
                token t(current[i].content, replace.end()-replace.begin(), 0);
                replace.emplace_back(t);
            }
        }
        current = replace;
        // If the first character of the current string is a terminal, execute a match.
        token first = current[0];
        if(std::find(terminals.begin(), terminals.end(), first.content) != terminals.end()){
            if(!match(inputCopy, current)){return false;}
        }
        if(current.empty() && !inputCopy.empty()){return false;}
    }
    // If both are empty then the parsing was a success.
    return true;
}

bool LL1Parser::match(std::vector<token>& input, std::vector<token>& current){
    int matches = 0;
    while(input[0].content == current[0].content){
        std::vector<token> newInput, newCurrent;
        matches += 1;
        std::copy(input.begin()+1, input.end(), newInput.begin());
        std::copy(current.begin()+1, current.end(), newCurrent.begin());
        input = newInput; current = newCurrent;
        if(input.empty() && current.empty()){return true;}
    }
    return matches > 0;
}


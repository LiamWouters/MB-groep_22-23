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
    token start(grammar.getStartState(), 0, 0);
    std::vector<std::vector<token>> current = {{start}};
    std::vector<std::vector<token>> progress = {input};
    // These variables were necessary to prevent some errors.
    std::vector<std::vector<std::string>> error = {{"<ERR>"}};
    token eos("<EOS>", 0, 0);
    // Step 2: Let the parsing begin!
    // As long as not both the input and current string are empty, operations must be done.
    for(int c = 0; c < current.size(); c++){
        // Keep in mind if a break has occurred
        bool Break = false;
        while(!(progress[c].empty() && current[c].empty())){
            std::vector<token> copy = current[c];
            // Fetch the first character (= "<EOS>" if string empty).
            token currentChar = progress[c][0];
            if(progress[c].empty()){currentChar = eos;}
            // At this point the first character of the current string is always a variable,
            // which means we expect a certain production.
            token currentSymbol = current[c][0];
            if(expect[std::make_pair(currentSymbol.content, currentChar.type)] == error){
                if(c == current.size()-1){return false;}
                Break = true; break;
            }
            for(int i = 0; i < expect[std::make_pair(currentSymbol.content, currentChar.type)].size(); i++){
                std::vector<token> replace;
                for(auto &j: expect[std::make_pair(currentSymbol.content, currentChar.type)][i]){
                    token t(j, 0, 0);
                    replace.emplace_back(t);
                }
                int counter = 0;
                for(auto &j: copy){
                    if(counter != 0){replace.emplace_back(j); counter += 1;}
                    else{counter += 1;}
                }
                if(i == 0){current[c] = replace;}
                else{current.emplace_back(replace); progress.emplace_back(progress[c]);}
            }
            // If the first character of the current string is a terminal, execute a match.
            token first = current[c][0];
            if(std::find(terminals.begin(), terminals.end(), first.content) != terminals.end()){
                if(!match(progress[c], current[c])){
                    if(c == current.size()-1){return false;}
                    Break = true; break;
                }
            }
            if(current.empty() && !progress[c].empty()){
                if(c == current.size()-1){return false;}
                Break = true; break;
            }
        }
        // If both are empty then the parsing was a success.
        if(!Break){return true;}
        else{continue;}
    }
}

bool LL1Parser::match(std::vector<token>& input, std::vector<token>& current){
    int matches = 0;
    while(input[0].type == current[0].content){
        matches += 1;
        pop_front(input);
        pop_front(current);
        if(input.empty() && current.empty()){return true;}
    }
    return matches > 0;
}

void LL1Parser::pop_front(std::vector<token> &v){
    std::reverse(v.begin(), v.end());
    v.pop_back();
    std::reverse(v.begin(), v.end());
}

token LL1Parser::getFront(std::vector<token> &v){
    return v[0];
}
//
// Created by axeld on 18/12/2022.
//

#include "LL1Parser.h"
#include <bits/stdc++.h>

LL1Parser::LL1Parser(const CFG &c) : grammar(c) {
    parseTable = c.lltable();
}

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
    token start(grammar.getStartState(), 0, 0);
    std::vector<std::vector<token>> current = {{start}};
    std::vector<std::vector<token>> progress = {input};
    token lastSymbol("NOTHING", {1,1});
    // Track the progress of each separate parse.
    std::map<int, int> totalMatches;
    std::vector<token> lastSymbols;
    // These variables were necessary to prevent some errors.
    std::vector<std::vector<std::string>> error = {{"<ERR>"}};
    token eos("<EOS>", 0, 0);
    // Step 2: Let the parsing begin!
    // As long as not both the input and current string are empty, operations must be done.
    for(int c = 0; c < current.size(); c++){
        // Keep in mind if a break has occurred
        bool Break = false;
        lastSymbols.emplace_back(lastSymbol);
        // Default matches is zero
        totalMatches[c] = 0;
        while(!(progress[c].empty() && current[c].empty())){
            std::vector<token> copy = current[c];
            // Fetch the first character (= "<EOS>" if string empty).
            token currentChar = progress[c][0];
            if(progress[c].empty()){currentChar = eos;}
            // At this point the first character of the current string is always a variable,
            // which means we expect a certain production.
            token currentSymbol = current[c][0];
            if(parseTable[std::make_pair(currentSymbol.content, currentChar.type)] == error){
                if(c == current.size()-1){
                    std::pair<unsigned int, unsigned int> most = mostProgress(totalMatches);
                    printErrorReport(std::make_pair(current[most.first], progress[most.first]), lastSymbols[c].content);
                    return false;
                }
                Break = true; break;
            }
            for(int i = 0; i < parseTable[std::make_pair(currentSymbol.content, currentChar.type)].size(); i++){
                std::vector<token> replace;
                for(auto &j: parseTable[std::make_pair(currentSymbol.content, currentChar.type)][i]){
                    token t(j, 0, 0);
                    replace.emplace_back(t);
                }
                int counter = 0;
                for(auto &j: copy){
                    if(counter != 0){replace.emplace_back(j); counter += 1;}
                    else{counter += 1;}
                }
                if(i == 0){current[c] = replace; lastSymbols[c] = currentSymbol;}
                else{current.emplace_back(replace); progress.emplace_back(progress[c]); lastSymbols.emplace_back(lastSymbols[c]);}
            }
            // If the first character of the current string is a terminal, execute a match.
            token first = current[c][0];
            if(std::find(grammar.t.begin(), grammar.t.end(), first.content) != grammar.t.end()){
                int matches = match(progress[c], current[c]);
                if(matches == 0){
                    if(c == current.size()-1){
                        std::pair<unsigned int, unsigned int> most = mostProgress(totalMatches);
                        printErrorReport(std::make_pair(current[most.first], progress[most.first]), lastSymbols[c].content);
                        return false;
                    }
                    Break = true; break;
                }
                else{totalMatches[c] += matches;}
            }
            if(current.empty() && !progress[c].empty()){
                if(c == current.size()-1){
                    std::pair<unsigned int, unsigned int> most = mostProgress(totalMatches);
                    printErrorReport(std::make_pair(current[most.first], progress[most.first]), lastSymbols[c].content);
                    return false;
                }
                Break = true; break;
            }
        }
        // If both are empty then the parsing was a success.
        if(!Break){return true;}
        else{continue;}
    }
    return true;
}

int LL1Parser::match(std::vector<token>& input, std::vector<token>& current){
    int matches = 0;
    while(input[0].type == current[0].content){
        matches += 1;
        pop_front(input);
        pop_front(current);
        if(input.empty() && current.empty()){return true;}
    }
    return matches;
}

void LL1Parser::pop_front(std::vector<token> &v){
    std::reverse(v.begin(), v.end());
    v.pop_back();
    std::reverse(v.begin(), v.end());
}

std::pair<unsigned int, unsigned int> LL1Parser::mostProgress(std::map<int, int> &totalMatches){
    unsigned int progress = 0;
    unsigned int index = 0;
    for(auto &t: totalMatches){
        if(t.second > progress){
            progress = t.second;
            index = t.first;
        }
    }
    return {index, progress};
}
void LL1Parser::printErrorReport(const std::pair<std::vector<token>, std::vector<token>>& cp, const std::string &c){
    // In this case, there are no errors, hence no need to report
    if(cp.second.empty()){return;}
    // Otherwise...
    token t = cp.second[0];
    std::string expected = cp.first[0].type;
    std::cout << "EXPECTED: " << expected;
}
//
// Created by axeld on 18/12/2022.
//

#include "LL1Parser.h"
#include <bits/stdc++.h>

LL1Parser::LL1Parser(const CFG &c) : grammar(c) {
    std::pair<std::map<std::string, std::vector<std::string>>, std::map<std::pair<std::string, std::string>, std::vector<std::vector<std::string>>>> table = c.lltable();
    parseTable = table.second;
    firstTable = table.first;
}

std::pair<bool, int> LL1Parser::accepts(const std::vector<token> &input){
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
            if(parseTable[std::make_pair(currentSymbol.content, currentChar.type)] == error || !grammar.isTerminal(currentChar.type)){
                if(c == current.size()-1){
                    std::vector<unsigned int> most = mostProgress(totalMatches, progress);
                    // int index = printErrorReport(std::make_pair(current, progress), strings, most, input);
                    return {false, input.size()-progress[most[0]].size()};
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
                        std::vector<unsigned int> most = mostProgress(totalMatches, progress);
                        // int index = printErrorReport(std::make_pair(current, progress), strings, most, input);
                        return {false, input.size()-progress[most[0]].size()};
                    }
                    Break = true; break;
                }
                else{totalMatches[c] += matches;}
            }
            if(current.empty() && !progress[c].empty()){
                if(c == current.size()-1){
                    std::vector<unsigned int> most = mostProgress(totalMatches, progress);
                    // int index = printErrorReport(std::make_pair(current, progress), strings, most, input);
                    return {false, input.size()-progress[most[0]].size()};
                }
                Break = true; break;
            }
        }
        // If both are empty then the parsing was a success.
        if(!Break){return {true, -1};}
        else{continue;}
    }
    return {true, -1};
}

int LL1Parser::match(std::vector<token>& input, std::vector<token>& current){
    /*
     * Matching function: given 2 vectors of tokens, this function will keep
     * popping the front token as long as the front tokens of both vectors match.
     * It returns an integer representing the amount of matches done.
     */
    int matches = 0;
    while(input[0].type == current[0].content){
        matches += 1;
        pop_front(input);
        pop_front(current);
        if(input.empty() && current.empty()){return 1;}
    }
    return matches;
}

void LL1Parser::pop_front(std::vector<token> &v){
    /*
     * A stupid function that makes popping the front of a vector possible.
     * Is this even efficient at all?
     */
    std::reverse(v.begin(), v.end());
    v.pop_back();
    std::reverse(v.begin(), v.end());
}

std::vector<unsigned int> LL1Parser::mostProgress(std::map<int, int> &totalMatches, std::vector<std::vector<token>>& v){
    /*
     * Track which sequences of derivations provided the most progress.
     */
    unsigned int index = 0;
    for(int t = 1; t < v.size(); t++){
        token one = v[t][0];
        token two = v[index][0];
        bool greaterLine = one.pos.line > two.pos.line;
        bool greaterColumn = one.pos.line == two.pos.line && one.pos.column > two.pos.column;
        if(greaterLine || greaterColumn){index = t;}
    }
    unsigned int mostProgress = v[index].size();
    std::vector<unsigned int> indexes{};
    for(int i = 0; i < v.size(); i++){
        if(v[i].size() == mostProgress){
            indexes.emplace_back(i);
        }
    }
    return indexes;
}

int LL1Parser::printErrorReport(const std::pair<std::vector<std::vector<token>>, std::vector<std::vector<token>>>& cp, const std::vector<std::string> &c, std::vector<unsigned int>& indexes, const std::vector<token>& input){
    /*
     * Print errors if there are any.
     */
    token t = cp.second[indexes[0]][0];
    std::cout << "ERROR at line " << t.pos.line << ", column " << t.pos.column << "\n";
    std::cout << "GOT: " << t.type << "\nExpected: ";
    std::vector<std::string> mentionedExpectations;
    for(auto &i: indexes){
        if(std::find(mentionedExpectations.begin(), mentionedExpectations.end(), cp.first[i][0].content) != mentionedExpectations.end()){
            continue;
        }
        if(grammar.isVariable(cp.first[i][0].content)){
            for(auto &j: firstTable[cp.first[i][0].content]){
                if(std::find(mentionedExpectations.begin(), mentionedExpectations.end(), j) != mentionedExpectations.end()){
                    continue;
                }
                mentionedExpectations.emplace_back(j);
            }
            continue;
        }
        mentionedExpectations.emplace_back(cp.first[i][0].content);
    }
    for(auto &i: mentionedExpectations){
        std::cout << i;
        if(i != *std::prev(mentionedExpectations.end())){
            std::cout << " / ";
        }
    }
    std::cout << std::endl;
    return input.size()-cp.second[indexes[0]].size();
}
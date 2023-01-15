//
// Created by axeld on 11/01/2023.
//

#include "CommandLineInterface.h"
#include "JsonTokenizer.h"
#include "EMLTokenizer.h"

static const CFG cJson("../res/json_grammar_simplified.json");
static const CFG cEml("../res/eml_grammar_simplified.json");
static LL1Parser llJson(cJson);
static LL1Parser llEml(cEml);
static LR1Parser lrJson(cJson, true);
static LR1Parser lrEml(cEml, true);
static EarleyParser earleyJson(cJson);
static EarleyParser earleyEml(cEml);

static std::map<std::string, std::string> descriptions = {
        {"h", "help command, shows currently valid user-input"},
        {"help", "help command, shows currently valid user-input"},
        {"e", "exit the program"},
        {"exit", "exit the program"},
        {"n", "when asked yes or no, this is a no-answer"},
        {"no", "when asked yes or no, this is a no-answer"},
        {"y", "when asked yes or no, this is a yes-answer"},
        {"yes", "when asked yes or no, this is a yes-answer"},
        {"p", "request a parse"},
        {"parse", "request a parse"},
        {"c", "request a file-conversion"},
        {"convert", "request a file-conversion"},
        {"s", "request a syntax-highlight"},
        {"syntax", "request a syntax-highlight"},
        {"ll", "parse a file with the ll1-parser"},
        {"earley", "parse a file the earley-way"},
        {"lr", "use the lr1 parser for parsing a file"}
};

static std::map<CommandLineInterface::state, std::string> messages = {
        {CommandLineInterface::start, "What would you like to do?\n"},
        {CommandLineInterface::yesNo, "Do you desire a syntax-highlighting output file?\n"},
        {CommandLineInterface::more, "Would you like to do something else?\n"},
        {CommandLineInterface::selectFile, "Choose a file:\n"},
        {CommandLineInterface::parserSelect, "Choose a parser:\n"}
};

static std::map<CommandLineInterface::state, std::vector<std::string>> validInputs = {
        {CommandLineInterface::start, {"p", "parse", "c", "convert", "s", "syntax", "h", "help", "e", "exit"}},
        {CommandLineInterface::yesNo, {"y", "yes", "n", "no", "h", "help", "e", "exit"}},
        {CommandLineInterface::more, {"y", "yes", "n", "no", "h", "help", "e", "exit"}},
        {CommandLineInterface::selectFile, {"file path", "e", "exit"}},
        {CommandLineInterface::parserSelect, {"ll", "lr", "earley", "e", "exit"}}
};

void CommandLineInterface::simulate(){
    state current = start;
    fileType type = none;
    parser p = null;
    bool fileChosen = false;
    bool parserChosen = false;
    bool parsing = false;
    bool highlighting = false;
    bool conversion = false;
    std::string file;
    while(current != exiting){
        std::string input;
        if(fileChosen && parserChosen && current != yesNo && current != more){
            parse(p, type, file);
            current = more;
            continue;
        }
        if(!fileChosen || current == yesNo || current == parserSelect || current == more){std::cout << messages[current];}
        std::cin >> input;
        for(auto &i: input){i = std::tolower(i);}
        if(!validCommand(current, input) && current != selectFile){
            std::cout << "\nInvalid command, expected:\n";
            help(current);
            continue;
        }
        if(current == more){
            if(input == "n" || input == "no"){current = exiting;}
            else{
                parsing = false;
                highlighting = false;
                conversion = false;
                fileChosen = false;
                parserChosen = false;
                current = start;
                p = null;
                type = none;
            }
        }
        if(current == parserSelect){
            if(input == "ll"){p = ll;}
            else if(input == "lr"){p = lr;}
            else if(input == "earley"){p = earley;}
            parserChosen = true;
            if(!highlighting){current = yesNo;}
            continue;
        }
        if(current == yesNo){
            current = parserSelect;
            continue;
        }
        if(current == selectFile){
            if(input == "e" || input == "exit"){current = exiting;}
            else if(input.substr(input.size()-5, 5) == ".json"){type = json;}
            else if(input.substr(input.size()-4, 4) == ".eml"){type = eml;}
            struct stat sb{};
            if(type == none || stat(input.c_str(), &sb) != 0){
                std::cout << "Invalid file path or file type, please provide a valid path to a json or eml file\n\n";
                continue;
            }
            file = input;
            fileChosen = true;
            current = parserSelect;
            continue;
        }
        else if(input == "h" || input == "help"){
            help(current);
            continue;
        }
        else if(input == "e" || input == "exit"){
            current = exiting;
            continue;
        }
        else if(input == "p" || input == "parse"){
            parsing = true;
            current = selectFile;
            continue;
        }
        else if(input == "c" || input == "convert"){
            conversion = true;
            parsing = true;
            current = selectFile;
            continue;
        }
        else if(input == "s" || input == "syntax"){
            highlighting = true;
            parsing = true;
            current = selectFile;
            continue;
        }
    }
    std::cout << "Exited\n";
}

void CommandLineInterface::help(const CommandLineInterface::state &s){
    std::vector<std::string> allowed = validInputs[s];
    std::cout << "Valid commands:\n";
    for(int i = 0; i < allowed.size(); i++){
        if(i != allowed.size()-1 && descriptions[allowed[i+1]] == descriptions[allowed[i]]){
            std::cout << allowed[i] << "/"; continue;
        }
        else{
            std::cout << allowed[i] << " - " << descriptions[allowed[i]] << "\n";
        }
    }
    std::cout << "\n";
}

bool CommandLineInterface::validCommand(const CommandLineInterface::state &s, const std::string &i){
    std::vector<std::string> valids = validInputs[s];
    for(auto &j: valids){
        if(j == i){return true;}
    }
    return false;
}

unsigned int CommandLineInterface::parse(const CommandLineInterface::parser &p, const CommandLineInterface::fileType &f, const std::string& v){
    JsonTokenizer j;
    EMLTokenizer e;
    bool success;
    if(f == json){j.tokenizeSimplified(v);}
    if(f == eml){e.tokenizeSimplified(v);}
    if(p == ll && f == json){
        success = llJson.accepts(j.tokens);
    }
    else if(p == ll && f == eml){
        success = llEml.accepts(e.tokens);
    }
    else if(p == lr && f == json){
        success = lrJson.parse(j.tokens);
    }
    else if(p == lr && f == eml){
        success = lrEml.parse(e.tokens);
    }
    else if(p == earley && f == json){
        success = earleyJson.validate(j.tokens);
    }
    else if(p == earley && f == eml){
        success = earleyJson.validate(e.tokens);
    }
    if(success){std::cout << "Parsing successful.\n";}
    else{std::cout << "Parsing failed.\n";}
}

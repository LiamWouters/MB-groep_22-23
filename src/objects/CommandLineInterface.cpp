//
// Created by axeld on 11/01/2023.
//

#include "CommandLineInterface.h"
#include "JsonTokenizer.h"
#include "EMLTokenizer.h"
#include "SyntaxHighlighter.h"
#include "JsonSchema.h"
#include "ParserComparison.h"
#include <dirent.h>

static const CFG cJson("../res/json_grammar_simplified.json");
static const CFG cEml("../res/eml_grammar_simplified.json");
static LL1Parser llJson(cJson);
static LL1Parser llEml(cEml);
static LR1Parser lrJson(cJson, true);
static LR1Parser lrEml(cEml, true);
static EarleyParser earleyJson(cJson);
static EarleyParser earleyEml(cEml);

/*
 * Conversion output file names.
 */
static std::map<CommandLineInterface::fileType, std::string> lookFor = {
        {CommandLineInterface::json, "EML-conversion-output.eml"},
        {CommandLineInterface::eml, "JSON-conversion-output.json"}
};

/*
 * Map with a description of all commands.
 */
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
        {"lr", "use the lr1 parser for parsing a file"},
        {"v", "validate a json file based on a schema of choice"},
        {"validate", "validate a json file based on a schema of choice"},
        {"file path", "path to an existing and valid file"},
        {"d", "get some interesting parse data from a file"},
        {"data", "get some interesting parse data from a file"}
};

/*
 * Map with messages that should be printed for each CLI state.
 */
static std::map<CommandLineInterface::state, std::string> msgs = {
        {CommandLineInterface::start, "\nWhat would you like to do?\n"},
        {CommandLineInterface::yesNo, "\nDo you desire a syntax-highlighting output file?\n"},
        {CommandLineInterface::more, "\nWould you like to do something else?\n"},
        {CommandLineInterface::selectFile, "\nChoose a file:\n"},
        {CommandLineInterface::parserSelect, "\nChoose a parser:\n"},
        {CommandLineInterface::schemaSelect, "\nChoose a schema:\n"}
};

/*
 * Map containing the valid inputs per state.
 */
static std::map<CommandLineInterface::state, std::vector<std::string>> validInputs = {
        {CommandLineInterface::start, {"p", "parse", "c", "convert", "s", "syntax", "h", "help", "e", "exit", "v", "validate", "d", "data"}},
        {CommandLineInterface::yesNo, {"y", "yes", "n", "no", "h", "help", "e", "exit"}},
        {CommandLineInterface::more, {"y", "yes", "n", "no", "h", "help", "e", "exit"}},
        {CommandLineInterface::selectFile, {"h", "help", "file path", "e", "exit"}},
        {CommandLineInterface::parserSelect, {"h", "help", "ll", "lr", "earley", "e", "exit"}},
        {CommandLineInterface::schemaSelect, {"e", "exit", "h", "help"}}
};

static std::map<std::string, JsonSchema> schemas;
/*
 * Look for valid schema's to be allowed for use in the CLI.
 */
void CommandLineInterface::getSchemas() {
    JsonTokenizer j; CFG c("../res/json_grammar_simplified.json");
    std::string schemaMap = "../res/schemas/";
    struct dirent *entry; DIR *dp;
    dp = ::opendir(schemaMap.c_str());
    if(dp == nullptr){std::cout << "Unexpected error: \"../res/schemas\" directory not found"; return;}
    while((entry = ::readdir(dp))){
        if(entry->d_name[0] == '.'){continue;}
        std::string s = schemaMap+entry->d_name;
        if(s.find(".json") == std::string::npos){continue;}
        j.tokenizeSimplified(s);
        if(lrJson.parse(j.tokens)){
            validInputs[schemaSelect].emplace_back(schemaMap+entry->d_name);
            descriptions[s] = "";
            schemas.insert({s, JsonSchema(s)});
        }
    }
    ::closedir(dp);
}

void CommandLineInterface::simulate(){
    /*
     * Run function.
     */
    // Useful variables
    getSchemas();
    state current = start;
    fileType type = none;
    parser p = null;
    bool fileChosen = false;
    bool parserChosen = false;
    bool highlighting = false;
    bool conversion = false;
    bool validation = false;
    bool stats = false;
    std::string file;
    std::string schema;
    // While not exited, request user input and act accordingly.
    while(current != exiting){
        std::string input;
        if(fileChosen && parserChosen && current != yesNo && current != more){
            parse(p, type, file, schema, conversion, highlighting, validation);
            current = more;
            continue;
        }
        if(!fileChosen || current == yesNo || current == parserSelect || current == more || current == schemaSelect){std::cout << msgs[current];}
        std::cin >> input;
        for(auto &i: input){i = std::tolower(i);}
        if(!validCommand(current, input) && current != selectFile){
            std::cout << "\nInvalid command, expected:\n";
            help(current);
            continue;
        }
        if(current == more){
            if(input == "h" || input == "help"){help(current); continue;}
            if(input == "n" || input == "no"){current = exiting;}
            else{
                highlighting = false;
                conversion = false;
                fileChosen = false;
                parserChosen = false;
                stats = false;
                current = start;
                p = null;
                type = none;
                file = "";
                schema = "";
                continue;
            }
        }
        if(current == parserSelect){
            if(input == "h" || input == "help"){help(current); continue;}
            if(input == "ll"){p = ll;}
            else if(input == "lr"){p = lr;}
            else if(input == "earley"){p = earley;}
            parserChosen = true;
            if(!highlighting){current = yesNo;}
            continue;
        }
        if(current == yesNo){
            if(input == "h" || input == "help"){help(current); continue;}
            if(input == "y" || input == "yes"){highlighting = true;}
            current = parserSelect;
            continue;
        }
        if(current == schemaSelect){
            if(input == "h" || input == "help"){help(current); continue;}
            if(input == "e" || input == "exit"){current = exiting;}
            if(std::find(validInputs[schemaSelect].begin(), validInputs[schemaSelect].end(), input) == validInputs[schemaSelect].end()){
                help(current); continue;
            }
            schema = input;
            current = parserSelect;
        }
        if(current == selectFile){
            if(input == "h" || input == "help"){help(current); continue;}
            if(input == "e" || input == "exit"){current = exiting; continue;}
            else if(input.substr(input.size()-5, 5) == ".json"){type = json;}
            else if(input.substr(input.size()-4, 4) == ".eml"){type = eml;}
            struct stat sb{};
            if(type == none || stat(input.c_str(), &sb) != 0 || (validation && type != json)){
                if(validation){std::cout << "Invalid file path or file type, please provide a valid path to a json file\n\n"; continue;}
                std::cout << "Invalid file path or file type, please provide a valid path to a json or eml file\n\n";
                continue;
            }
            file = input;
            fileChosen = true;
            if(conversion){
                parserChosen = true;
                p = lr;
                current = yesNo;
            }
            else if(validation){current = schemaSelect;}
            else if(stats){
                compareAllParsers(file, 1);
                current = more;
            }
            else{current = parserSelect;}
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
            current = selectFile;
            continue;
        }
        else if(input == "c" || input == "convert"){
            conversion = true;
            current = selectFile;
            continue;
        }
        else if(input == "s" || input == "syntax"){
            highlighting = true;
            current = selectFile;
            continue;
        }
        else if(input == "v" || input == "validate"){
            validation = true;
            current = selectFile;
            continue;
        }
        else if(input == "d" || input == "data"){
            stats = true;
            current = selectFile;
        }
    }
    std::cout << "---> Exited. Make sure to save your output files elsewhere!\n";
}

void CommandLineInterface::help(const CommandLineInterface::state &s){
    /*
     * Request the currently valid inputs.
     */
    std::vector<std::string> allowed = validInputs[s];
    std::cout << "Valid commands:\n";
    if(s == schemaSelect){
        std::cout << "e/exit - " << descriptions["e"] << "\n";
        std::cout << "h/help - " << descriptions["h"] << "\n\n";
        std::cout << "Valid schema\'s:\n";
        for(auto &i: validInputs[schemaSelect]){
            if(i == "e" || i == "exit" || i == "h" || i == "help"){continue;}
            std::cout << i << "\n";
        }
        return;
    }
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
    /*
     * Check if a command is valid for the current state.
     */
    std::vector<std::string> valids = validInputs[s];
    for(auto &j: valids){
        if(j == i){return true;}
    }
    return false;
}

void CommandLineInterface::parse(const CommandLineInterface::parser &p, const CommandLineInterface::fileType &f, const std::string& v, const std::string& s, bool& converse, bool& highlight, bool& validation){
    /*
     * Parse, convert and/or syntax highlight a file.
     */
    JsonTokenizer j; EMLTokenizer e;
    std::pair<bool, int> success{true, -1};
    if(f == json){
        j.tokenizeSimplified(v);
        earleyJson.m_chart.clear();
        earleyJson.m_input = j.tokens;
        earleyJson.initChart();
        earleyJson.fillChart();
    }
    if(f == eml){
        e.tokenizeSimplified(v);
        earleyEml.m_chart.clear();
        earleyEml.m_input = e.tokens;
        earleyEml.initChart();
        earleyEml.fillChart();
    }
    if(p == ll && f == json){
        success = llJson.accepts(j.tokens);
        if(!success.first){
            earleyJson.printErrorReport(Json, v, std::cout);
        }
    }
    else if(p == ll && f == eml){
        success = llEml.accepts(e.tokens);
        if(!success.first){
            earleyEml.printErrorReport(EML, v, std::cout);
        }
    }
    else if(p == lr && f == json){
        success.first = lrJson.parse(j.tokens);
        if(!success.first){
            earleyJson.printErrorReport(Json, v, std::cout);
            success.second = earleyJson.m_chart.size()-1;
        }
    }
    else if(p == lr && f == eml){
        success.first = lrEml.parse(e.tokens);
        if(!success.first){
            earleyEml.printErrorReport(EML, v, std::cout);
            success.second = earleyEml.m_chart.size()-1;
        }
    }
    else if(p == earley && f == json){
        success.first = earleyJson.validateFile(v, Json);
        if(!success.first){success.second = earleyJson.m_chart.size()-1;}
    }
    else if(p == earley && f == eml){
        success.first = earleyEml.validate(e.tokens);
        if(!success.first){success.second = earleyEml.m_chart.size()-1;}
    }
    if(success.first){
        std::cout << "\n---> Parsing successful.\n";
        if(converse){
            if(f == json){
                lrJson.printToEML();
            }
            else{lrEml.printToJSON();}
            std::cout << "---> Conversion successful, look for " << lookFor[f] << " in the \"res\" folder.\n";
            std::cout << "---> Make sure to save it somewhere else.\n";
        }
        if(validation){
            JsonSchema js(s);
            bool b = js.validate(v);
            if(b){std::cout << "---> Validation success.";}
            if(!b){std::cout << "---> Validation failed.";}
        }
    }
    else{
        std::cout << "\n---> Parsing failed.\n";
        if(converse){std::cout << "---> Conversion aborted.\n";}
        if(validation){std::cout << "---> Schema-validation aborted.\n";}
    }
    if(highlight){
        if(f == json){
            SyntaxHighlighter::jsonToHTML2(j.tokens, success.second, outputs);
            std::cout << "---> output" + to_string(outputs) << ".html created in the \"res\" folder\n";
            outputs += 1;
        }
        if(f == eml){
            SyntaxHighlighter::customToHTML2(e.tokens, success.second, outputs);
            std::cout << "---> output" + to_string(outputs) << ".html created in the \"res\" folder\n";
            outputs += 1;
        }
        if(converse && success.first){
            if(f == json){
                e.tokenizeSimplified("../res/EML-conversion-output.eml");
                SyntaxHighlighter::customToHTML2(e.tokens, success.second, outputs);
                std::cout << "---> output" + to_string(outputs) << ".html created in the \"res\" folder\n";
                outputs += 1;
            }
            else{
                j.tokenizeSimplified("../res/JSON-conversion-output.json");
                SyntaxHighlighter::jsonToHTML2(j.tokens, success.second, outputs);
                std::cout << "---> output" + to_string(outputs) << ".html created in the \"res\" folder\n";
                outputs += 1;
            }
        }
    }
}

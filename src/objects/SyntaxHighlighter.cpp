//
// Created by axeld on 15/12/2022.
//
#include <bits/stdc++.h>
#include "SyntaxHighlighter.h"
#include "JsonTokenizer.h"
// Makes sure that the html takes spaces into account
#define pre(x) ("<pre>"+(x)+"</pre>");
// Body
#define body "<body>"
// Define some colors for the syntax highlighting (black is default and doesn't require one)
#define red "<span style=\"color:red;\">"
#define green "<span style=\"color:green;\">"
#define blue "<span style=\"color:blue;\">"
#define pink "<span style=\"color:deeppink;\">"
#define purple "<span style=\"color:purple;\">"
#define black ""
// Define the underline tag
#define underline "<u>"

/*
 * Colours all get an enumeration, mostly to make the code more readable.
 * A map shows which token types get assigned which color.
 *
 * This does not contain token types that use multiple colors.
 */
enum Colour{Red, Green, Blue, Pink, Black, Purple};
std::map<Colour, std::string> colourMap = {{Red, red}, {Green, green}, {Blue, blue},
                                           {Pink, pink}, {Black, black}, {Purple, purple}};
std::vector<std::string> digits = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "."};

std::map<std::string, std::string> colourUsage {
    {"COMMA", black},
    {"ARRAY_OPEN", black},
    {"ARRAY_CLOSE", black},
    {"CURLY_OPEN", black},
    {"CURLY_CLOSE", black},
    {"COLON", black},
    {"NUMBER", purple},
    {"NULL", pink},
    {"BOOLEAN", pink},
    {"STRING", green},
    {"UNKNOWN", red}
};

/*
 * A simple function that returns the endtag corresponding to the given input tag.
 */
std::string endTag(const std::string& tag){
    for(auto &i: colourMap){
        if(i.second == tag){return "</span>";}
    }
    return "</"+tag.substr(1, tag.size()-1);
}

/*
 * NOTE: the functions with the path argument are first drafts and lack some functionality,
 * it is better to use the ones that require an input token-vector and index instead.
 */
void SyntaxHighlighter::jsonToHTML(const std::string &path){
    /*
     * Given a path to a JSON file, this function will create an HTML output file that highlights
     * the syntax of the file.
     *
     * Used Colors:
     * Blue: Non-value strings
     * Green: Value strings
     * Purple: Integer
     * Pink: Boolean
     * Red: Error (any)
     * Black: Characters such as '{}', ',' and '[]'
     *
     * This method is static since an instance of this class is not really required, it is intended
     * to be allowed to be called on its own.
     *
     * Currently, no actual errors will be marked yet, since our parsers still need to find a way to deliver
     * the errors to here.
     *
     * =====================================================================
     *
     * Step 1: Get the filename from the given path and check if it's JSON.
     */
    std::string copy = path;
    while(copy.find('/') != std::string::npos){
        copy = copy.substr(1, copy.size()-1);
    }
    if(!copy.find(".json")){std::cout << "Invalid path: expected JSON file.\n"; return;}
    for(int i = 0; i < 5; i++){copy.pop_back();}
    // Step 2: Create an HTML file with the same filename in the outputhtml folder.
    std::ofstream file; file.open("../outputhtml/"+copy+".html", std::ios::out);
    if(!file){std::cout << "Error in creating html file: perhaps an invalid path?\n"; return;}
    // Final Step: Syntax Highlight and write to HTML with the help of a tokenizer.
    JsonTokenizer j; j.tokenize(path);
    // Keep track of the current line number and its contents.
    int currLine = 1; std::string addLine;
    // Some bools for conditions, the clones deal with recursion.
    bool inQuotes, inValue, inArray, inValueClone, inArrayClone = false;
    // Keep track of the current color to generalize placing color end tags.
    Colour currentColour = Black;
    // A clean HTML file has a body tag.
    file << body;
    // This is just a question of: Where am I in the file? A value? A string? An array? And which current character?
    for(auto i = j.tokens.begin(); i != j.tokens.end(); i++){
        if(i->pos.line == currLine+1){
            file << pre(addLine)
            addLine = "";
            currLine += 1;
        }
        if(i->content == "\"" && !(std::prev(i)->content == "\\")){
            inQuotes = !inQuotes;
            if(inQuotes && !inValue){addLine += blue; currentColour = Blue;}
            else if(inQuotes){addLine += green; currentColour = Green;}
        }
        while(addLine.size()<i->pos.column-1){addLine += " ";}
        if(i->content == "," && !inQuotes && inValue && !inArray){inValue = false;}
        if(i->content == "{" && !inQuotes && inArray && inValue){
            inArrayClone = inArray; inValueClone = inValue;
            inValue = false; inArray = false;
        }
        if(i->content == "}" && !inQuotes){
            inValue = inValueClone; inArray = inArrayClone;
        }
        if(inValue && !inQuotes || inArray && !inQuotes){
            std::string c = i->content;
            if(!(c == "," || c == "{" || c == ":" || c == "[" || c == "]" || c == "\"" || c == "}")){
                if(std::find(digits.begin(), digits.end(), c) != digits.end()){currentColour = Purple; addLine += purple;}
                else{currentColour = Pink; addLine += pink;}
            }
        }
        addLine += i->content;
        if(i->content == "[" || i->content == "]"){inArray = (i->content == "["); if(!inArray){inValue = false;}}
        if(i->content == ":" && !inQuotes){inValue = true;}
        if(i->content == "\"" && !inQuotes){addLine += endTag(colourMap[currentColour]);}
        if(currentColour == Pink || currentColour == Purple){currentColour = Black; addLine += endTag(colourMap[Pink]);}
        if(i == std::prev(j.tokens.end())){file << pre(addLine)}
    }
    file << endTag(body); file.close();
    std::cout << copy+".html" << " created successfully!\n";
}

void SyntaxHighlighter::customToHTML(const std::string& path){
    /*
     * Given a path to a Custom Markup Language (CML) file, this function creates an output HTML
     * that highlights the syntax of the file.
     *
     * Used colors:
     * Purple: Characters inside tags.
     * Blue: Characters such as '<', '>' and '-'.
     * Black: Characters between tags.
     * Red: Errors (any)
     *
     * =====================================================================
     *
     * Step 1: Get the filename from the given path and check if it's CML.
     */
    std::string copy = path;
    while(copy.find('/') != std::string::npos){
        copy = copy.substr(1, copy.size()-1);
    }
    if(!copy.find(".cml")){std::cout << "Invalid path: expected CML file.\n"; return;}
    for(int i = 0; i < 4; i++){copy.pop_back();}
    // Step 2: Create an HTML file with the same filename in the outputhtml folder.
    std::ofstream file; file.open("../outputhtml/"+copy+".html", std::ios::out);
    if(!file){std::cout << "Error in creating html file: perhaps an invalid path?\n"; return;}
    // Final Step: Syntax Highlight and write to HTML with the help of a tokenizer.
    JsonTokenizer j; j.tokenize(path);
    // Keep track of the current line number and its contents.
    int currLine = 1; std::string addLine; std::string tagContent;
    // Some bools for conditions, the clones deal with recursion.
    bool betweenTag = false;
    // A clean HTML file has a body tag.
    file << body;
    // Iterate over all tokens and color them accordingly, main obstacles are '<', '/' and '>'.
    // Notice the use of &lt; and &gt;, these make it possible to actually display "tags".
    for(auto & token : j.tokens){
        if(token.pos.line == currLine+1){
            currLine += 1;
            file << pre(addLine)
            addLine = "";
        }
        while(addLine.size()<token.pos.column-1){addLine += " ";}
        if(token.content == "/"){
            addLine += endTag(purple); addLine += blue; addLine += token.content; addLine += endTag(blue); addLine += purple;
            continue;
        }
        if(token.content == "<"){
            addLine += blue; addLine += "&lt;"; addLine += purple;
            continue;
        }
        if(token.content == ">"){
            betweenTag = !betweenTag;
            addLine += endTag(purple); addLine += "&gt;"; addLine += endTag(blue);
            continue;
        }
        addLine += token.content;
    }
    // Add the final line and the body endtag.
    file << pre(addLine) file << endTag(body);
}

void SyntaxHighlighter::jsonToHTML2(std::vector<token> &t, int& index){
    /*
     * Given a sequence of json-tokens and a possible error index,
     * this function creates a syntax-highlighted output file.
     */
    // Some helpful variables.
    int counter = 0;
    int arrayStack = 0;
    int curlyStack = 0;
    int currLine = 1;
    std::string addLine;
    // Create, open an output file and add the body.
    std::ofstream file;
    file.open("../res/output.html", std::ios::out);
    file << body;
    // Iterate over each token, add them to the output in the correct color.
    for(auto &i: t){
        if(i.pos.line == currLine+1){
            currLine += 1;
            file << pre(addLine)
            addLine = "";
        }
        while(addLine.size()<i.pos.column-1){addLine += " ";}
        if(i.type == "ARRAY_OPEN"){arrayStack += 1;}
        if(i.type == "ARRAY_CLOSE"){arrayStack -= 1;}
        if(i.type == "CURLY_OPEN"){curlyStack += 1;}
        if(i.type == "CURLY_CLOSE"){curlyStack -= 1;}
        if(counter == index || i.type == "UNKNOWN"){
            addLine += red;
            addLine += underline; addLine += i.content; addLine += endTag(underline);
            addLine += endTag(red);
        }
        else if(i.type == "STRING"){
            token next = t[counter+1];
            if(next.type == "COLON" && index != counter+1){
                addLine += blue; addLine += i.content; addLine += endTag(blue);
            }
            else{
                addLine += green; addLine += i.content; addLine += endTag(green);
            }
        }
        else{addLine += colourUsage[i.type]; addLine += i.content; addLine += endTag(colourUsage[i.type]);}
        counter += 1;
    }
    // Add the final line and the body endtag.
    file << pre(addLine) file << endTag(body);
}

void SyntaxHighlighter::customToHTML2(std::vector<token> &t, int& index){
    /*
     * Given a sequence of eml-tokens and a possible error index,
     * this function creates a syntax-highlighted output file.
     */
    // Some useful variables.
    int counter = 0;
    int currLine = 1;
    std::string addLine;
    std::ofstream file;
    // Create, open an output file and add the body.
    file.open("../res/output.html", std::ios::out);
    file << body;
    // Iterate over each token, add them to the output and color them correctly.
    for(auto &i: t){
        if(i.pos.line == currLine+1){
            currLine += 1;
            file << pre(addLine)
            addLine = "";
        }
        while(addLine.size()<i.pos.column-1){addLine += " ";}
        if(counter == index || i.type == "UNKNOWN"){
            addLine += red;
            addLine += underline; addLine += i.content; addLine += endTag(underline);
            addLine += endTag(red);
        }
        else if(i.type == "ARRAY_TAG_OPEN"){
            addLine += blue; addLine += "&lt"; addLine += purple;
            for(int j = 1; j < i.content.size()-1; j++){
                addLine += i.content[j];
            }
            addLine += endTag(purple); addLine += "&gt"; addLine += endTag(blue);
        }
        else if(i.type == "ARRAY_TAG_CLOSE"){
            addLine += blue; addLine += "&lt/"; addLine += purple;
            for(int j = 2; j < i.content.size()-1; j++){
                addLine += i.content[j];
            }
            addLine += endTag(purple); addLine += "&gt"; addLine += endTag(blue);
        }
        else if(i.type == "TAG_OPEN" || i.type == "ROOT_OPEN"){
            addLine += blue; addLine += "["; addLine += purple;
            for(int j = 1; j < i.content.size()-1; j++){
                addLine += i.content[j];
            }
            addLine += endTag(purple); addLine += "]"; addLine += endTag(blue);
        }
        else if(i.type == "TAG_CLOSE" || i.type == "ROOT_CLOSE"){
            addLine += blue; addLine += "[/"; addLine += purple;
            for(int j = 2; j < i.content.size()-1; j++){
                addLine += i.content[j];
            }
            addLine += endTag(purple); addLine += "]"; addLine += endTag(blue);
        }
        else{addLine += colourUsage[i.type]; addLine += i.content; addLine += endTag(colourUsage[i.type]);}
        counter += 1;
    }
    // Add the final line and the body endtag.
    file << pre(addLine) file << endTag(body);
}
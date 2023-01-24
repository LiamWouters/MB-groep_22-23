//
// Created by michel on 22.12.22.
//

#include "EarleyParser.h"
#include "../../utilities/utilities.h"
#include "../JsonTokenizer.h"
#include "../EMLTokenizer.h"
#include <fstream>
#include <unordered_set>

EarleyParser::EarleyParser(const CFG& grammar) : m_grammar{grammar} { init(); }

EarleyParser::EarleyParser(const std::string& path_to_grammar) : m_grammar(path_to_grammar) { init(); }

void EarleyParser::initChart() {
    // initialize first StateSet
    for (const auto& prod : m_grammar.p) {
        // find productions with starting symbol as head
        if (prod.head == m_grammar.s) {
            // add production to chart
            m_chart.emplace_back();
            m_chart[0].addEarleyItemUnsafe({prod, 0, 0});
        }
    }
}

void EarleyParser::predict(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set) {
    // find productions with next_symbol as head
    for (const auto& prod : m_grammar.p) {
        if (prod.head == next_symbol) {
            // add to chart
            m_chart[index_chart].addEarleyItem({prod, 0, index_chart});

            // special completion to handle nullables
            if (isNullable(prod.head)) {
                // get item
                EarleyItem to_complete = getEarlyItem(index_chart, index_state_set);
                m_chart[index_chart].addEarleyItem(
                    {to_complete.m_production, to_complete.m_dot + 1, to_complete.m_start});
            }
        }
    }
}

void EarleyParser::scan(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set) {
    // compare next_symbol with input
    // old rule: if (next_symbol == m_input[index_chart].content)
    if (next_symbol == m_input[index_chart].type) {
        EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);
        // add StateSet to chart if needed (make chart array 1 bigger)
        if (m_chart.size() - 1 == index_chart) {
            m_chart.emplace_back();
        }
        // add cur EarleyItem in next StateSet with dot one place advanced
        m_chart[index_chart + 1].addEarleyItemUnsafe({cur_item.m_production, cur_item.m_dot + 1, cur_item.m_start});
    }
}

void EarleyParser::complete(unsigned int index_chart, unsigned int index_state_set) {
    EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);

    // look for productions to be completed
    for (auto& item : m_chart[cur_item.m_start].m_set) {
        if (item.isDotAtEnd()) {
            continue;
        }
        if (cur_item.m_production.head == item.getNextSymbol()) {
            // complete
            m_chart[index_chart].addEarleyItem({item.m_production, item.m_dot + 1, item.m_start});
        }
    }
}

EarleyItem EarleyParser::getEarlyItem(unsigned int index_chart, unsigned int index_state_set) const {
    return m_chart[index_chart].m_set[index_state_set];
}
void EarleyParser::printChart(std::ostream& out) const {
    for (unsigned int index_chart = 0; index_chart < m_chart.size(); index_chart++) {
        out << "=== " << index_chart << " ===" << std::endl;
        m_chart[index_chart].print(out);
        out << std::endl;
    }
}

void EarleyParser::printChartToFile(const std::string& path) const {
    std::ofstream file(path);
    if (file.is_open()) {
        printChart(file);
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

void EarleyParser::init() { m_nullables = m_grammar.findNullableVariables(); }

bool EarleyParser::isNullable(const std::string& variable) const { return stringInSet(variable, m_nullables); }

void EarleyParser::fillChart() {
    for (unsigned int index_chart = 0; index_chart < m_chart.size(); index_chart++) {
        for (unsigned int index_state_set = 0; index_state_set < m_chart[index_chart].m_set.size(); index_state_set++) {
            addItemToChart(index_chart, index_state_set);
        }
    }
}
void EarleyParser::addItemToChart(unsigned int index_chart, unsigned int index_state_set) {
    if (m_chart[index_chart].m_set[index_state_set].isDotAtEnd()) {
        complete(index_chart, index_state_set);
    } else {
        std::string next_symbol = m_chart[index_chart].m_set[index_state_set].getNextSymbol();
        if (m_grammar.isVariable(next_symbol)) {
            predict(next_symbol, index_chart, index_state_set);
        } else {
            scan(next_symbol, index_chart, index_state_set);
        }
    }
}

bool EarleyParser::validate(const std::vector<token>& input) {
    m_chart.clear();
    m_input = input;
    initChart();
    fillChart();
    return has_complete_parse();
}

bool EarleyParser::validateFile(const std::string& path, ML markUpLanguage) {
    if (markUpLanguage == Json) {
        JsonTokenizer tok_json;
        tok_json.tokenizeSimplified(path);
        return validate(tok_json.tokens);
    }
    else if (markUpLanguage == EML) {
        EMLTokenizer tok_eml;
        tok_eml.tokenizeSimplified(path);
        return validate(tok_eml.tokens);
    }
    // if statements for other languages here
    return false;
}

bool EarleyParser::has_partial_parse(unsigned int index_chart) const {
    for (unsigned int index_state_set = 0; index_state_set < m_chart[index_chart].m_set.size(); index_state_set++) {
        EarleyItem cur_item = getEarlyItem(index_chart, index_state_set);
        if (cur_item.m_production.head == m_grammar.s && cur_item.isDotAtEnd() && cur_item.m_start == 0) {
            return true;
        }
    }
    return false;
}

bool EarleyParser::has_complete_parse() const { return has_partial_parse(m_chart.size() - 1); }

unsigned int EarleyParser::get_index_last_partial_parse() const {
    unsigned int index_last_partial_parse = 0;
    for (unsigned int index_chart = 0; index_chart < m_chart.size(); index_chart++) {
        if (has_partial_parse(index_chart)) {
            index_last_partial_parse = index_chart;
        }
    }
    return index_last_partial_parse;
}

void EarleyParser::printErrorReport(ML MarkUpLanguage, const std::string& fileWithError, std::ostream& out) const {
    if (MarkUpLanguage == Json) {
        getErrorReportJson(fileWithError, out);
    } else if (MarkUpLanguage == EML){
        getErrorReportJson(fileWithError, out);
    }
}

void EarleyParser::printErrorReportToFile(ML MarkUpLanguage, const std::string& fileWithError,
                                          const std::string& path) const {
    std::ofstream file(path);
    if (file.is_open()) {
        printErrorReport(MarkUpLanguage, fileWithError, file);
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

void EarleyParser::getErrorReportJson(const std::string& fileWithError, std::ostream& out) const {
    unsigned int chart_size = m_chart.size();
    token unexpected_token = m_input[chart_size - 1];

    // print where error occurred
    out << "Unexpected token of type \'" << unexpected_token.type << "\' with content \'" << unexpected_token.content
        << "\' at line " << unexpected_token.pos.line << " and column " << unexpected_token.pos.column << "."
        << std::endl
        << std::endl;

    // print the json until error occured (with token after error present)
    readFile(unexpected_token.pos.line, unexpected_token.pos.column + unexpected_token.content.size() - 1,
             fileWithError, out);

    out << std::endl;
    out << "Instead of a " << unexpected_token.type << ", the parser expected a: " << std::endl;

    // print expected token
    // look in last state set
    std::vector<EarleyItem> items_to_explore; // vector to keep track of potential tracked tokens
    for (const auto& item : m_chart.back().m_set) {
        // if dot is at end or element next to dot is not a terminal: continue
        if (item.isDotAtEnd()) {
            continue;
        }
        std::string expected_token = item.getNextSymbol();
        if (m_grammar.isVariable(expected_token)) {
            continue;
        }
        // if symbol next to dot is a terminal, store rules we will explore in items_to_explore

        // check if terminal is already encountered, if so: don't add to items_to_explore
        bool already_encountered = false;
        for (const auto& an_item : items_to_explore) {
            if (item.getNextSymbol() == an_item.getNextSymbol()) {
                already_encountered = true;
            }
        }
        if (already_encountered) {
            continue;
        }
        items_to_explore.emplace_back(item);
    }
    // loop over items to explore:
    for (unsigned int i = 0; i < items_to_explore.size(); i++) {

        EarleyItem cur_item = items_to_explore[i];
        out << std::endl;
        // what terminal is expected?
        out << cur_item.getNextSymbol() << ", based on:" << std::endl;

        unsigned int chart_index = chart_size - 1;
        bool not_reached_end = true;

        std::vector<std::string> rules;
        insertStringInVectorIfLastIsDifferent(rules, items_to_explore[i].m_production.toString());

        // while we have not found the start tag
        while (not_reached_end) {
            // loop over items in current chart position, until fitting rule is found
            for (const auto& state_set_item : m_chart[chart_index].m_set) {
                if (!state_set_item.isDotAtEnd()) {
                    if (state_set_item.getNextSymbol() == cur_item.m_production.head) {
                        // new item to explore
                        cur_item = state_set_item;
                        // add rule of new item to rules
                        insertStringInVectorIfLastIsDifferent(rules, cur_item.m_production.toString());
                        // if we have found the start tag, stop exploring
                        if (cur_item.m_production.head == m_grammar.s) {
                            not_reached_end = false;
                        }
                        // if we have found a matching rule, stop exploring
                        break;
                    }
                }
            }
            if (cur_item.m_dot != 0) {
                // if dot is not at start position, go to position in chart indicated by start field item
                chart_index = cur_item.m_start;
            }
        }
        for (const auto& str : rules) {
            // print rules that explain current expected token
            out << '\t' << str << std::endl;
        }
    }
}

void EarleyParser::readFile(const unsigned int l, const unsigned int c, const std::string& fileWithError,
                            std::ostream& out) const {
    std::ifstream file(fileWithError);
    if (file.is_open()) {
        std::string line;
        std::string output;
        unsigned int line_counter = 0;
        while (std::getline(file, line) and line_counter < l) {
            if (line_counter == l - 1) {
                for (unsigned int i = 0; i < line.size(); i++) {
                    if (i < c) {
                        output += line[i];
                    }
                }
            } else {
                output += line;
                output += '\n';
            }
            line_counter++;
        }
        out << output;
        out << std::endl;
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

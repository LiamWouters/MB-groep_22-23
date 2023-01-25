//
// Created by michel on 22.12.22.
//

#ifndef MB_GROEP_22_23_EARLEYPARSER_H
#define MB_GROEP_22_23_EARLEYPARSER_H

#include "../CFG.h"
#include "../Token.h"
#include "StateSet.h"

enum ML { Json, EML };

class EarleyParser {
public:
    /// constructors
    EarleyParser(const CFG& grammar);

    EarleyParser(const std::string& path_to_grammar);

    /// initializer
    void init();

    /// member functions

    void initChart(); // initializes chart
    void predict(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set);
    void scan(const std::string& next_symbol, unsigned int index_chart, unsigned int index_state_set);
    void complete(unsigned int index_chart, unsigned int index_state_set);

    void fillChart();
    void addItemToChart(unsigned int index_chart, unsigned int index_state_set);

    /// print functions
    void printChart(std::ostream& out = std::cout) const;
    void printChartToFile(const std::string& path) const;

    /// validation
    bool has_partial_parse(unsigned int index_chart) const;
    bool has_complete_parse() const;
    unsigned int get_index_last_partial_parse() const;
    bool validate(const std::vector<token>& input);

    bool validateFile(const std::string& path,
                      ML markUpLanguage); // tokenizes and validates (based on grammar in constructor)

    /// error reporting
    void printErrorReport(ML MarkUpLanguage, const std::string& fileWithError, std::ostream& out = std::cout) const;
    void printErrorReportToFile(ML MarkUpLanguage, const std::string& fileWithError, const std::string& path) const;

    void readFile(unsigned int l, unsigned int c, const std::string& fileWithError,
                  std::ostream& out = std::cout) const;

    /// variables
    std::vector<StateSet> m_chart;
    CFG m_grammar;
    std::vector<token> m_input;
    std::set<std::string> m_nullables;

private:
    /// helper functions
    EarleyItem getEarlyItem(unsigned int index_chart, unsigned int index_state_set) const;
    bool isNullable(const std::string& variable) const;
    void getErrorReport(ML MarkUpLanguage, const std::string& fileWithError, std::ostream& out = std::cout) const;
};

#endif // MB_GROEP_22_23_EARLEYPARSER_H

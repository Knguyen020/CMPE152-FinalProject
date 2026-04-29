#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <string>
#include <vector>
#include <unordered_map>
#include "token.h"

class SemanticAnalyzer {
private:
    std::vector<Token> tokens;
    std::vector<std::string> errors;
    std::vector<std::string> checkResults;
    std::unordered_map<std::string, int> stats;

    bool isValidStatName(const std::string& name) const;
    bool isNumber(const std::string& text) const;

    void extractAssignments();
    void validateRules();

    int getTermValue(size_t index, bool& ok);
    int evaluateExpression(size_t startIndex, size_t endIndex, bool& ok);
    bool evaluateComparison(int leftValue, TokenType op, int rightValue) const;
    void evaluateChecks();

public:
    SemanticAnalyzer(const std::vector<Token>& tokenList);

    void analyze();
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getCheckResults() const;
    const std::unordered_map<std::string, int>& getStats() const;
};

#endif
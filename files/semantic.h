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
    std::unordered_map<std::string, int> stats;

    bool isValidStatName(const std::string& name) const;
    bool isNumber(const std::string& text) const;
    void extractAssignments();
    void validateRules();

public:
    SemanticAnalyzer(const std::vector<Token>& tokenList);

    void analyze();
    const std::vector<std::string>& getErrors() const;
    const std::unordered_map<std::string, int>& getStats() const;
};

#endif
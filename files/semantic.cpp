#include "semantic.h"
#include <cctype>

SemanticAnalyzer::SemanticAnalyzer(const std::vector<Token>& tokenList)
    : tokens(tokenList) {}

bool SemanticAnalyzer::isValidStatName(const std::string& name) const {
    return name == "min" ||
           name == "pts" ||
           name == "reb" ||
           name == "ast" ||
           name == "fgm" ||
           name == "fga" ||
           name == "three_pm" ||
           name == "three_pa" ||
           name == "ftm" ||
           name == "fta" ||
           name == "stl" ||
           name == "blk" ||
           name == "to" ||
           name == "pf" ||
           name == "plus_minus";
}

bool SemanticAnalyzer::isNumber(const std::string& text) const {
    if (text.empty()) {
        return false;
    }

    size_t start = 0;

    if (text[0] == '-' || text[0] == '+') {
        if (text.length() == 1) {
            return false;
        }
        start = 1;
    }

    for (size_t i = start; i < text.length(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
            return false;
        }
    }

    return true;
}

void SemanticAnalyzer::extractAssignments() {
    for (size_t i = 0; i + 2 < tokens.size(); i++) {
        if (tokens[i].type == TokenType::IDENTIFIER &&
            tokens[i + 1].type == TokenType::ASSIGN &&
            tokens[i + 2].type == TokenType::NUMBER) {

            std::string statName = tokens[i].lexeme;
            std::string valueText = tokens[i + 2].lexeme;

            if (!isValidStatName(statName)) {
                errors.push_back(
                    "Line " + std::to_string(tokens[i].line) +
                    ": Invalid stat name '" + statName + "'"
                );
                continue;
            }

            if (!isNumber(valueText)) {
                errors.push_back(
                    "Line " + std::to_string(tokens[i + 2].line) +
                    ": Invalid numeric value '" + valueText + "'"
                );
                continue;
            }

            int value = std::stoi(valueText);

            // Allow plus_minus to be negative, but all other stats must be nonnegative
            if (statName != "plus_minus" && value < 0) {
                errors.push_back(
                    "Line " + std::to_string(tokens[i + 2].line) +
                    ": Stat '" + statName + "' cannot be negative"
                );
                continue;
            }

            stats[statName] = value;
        }
    }
}

void SemanticAnalyzer::validateRules() {
    if (stats.count("fgm") && stats.count("fga")) {
        if (stats["fga"] < stats["fgm"]) {
            errors.push_back("Semantic error: fga cannot be less than fgm");
        }
    }

    if (stats.count("three_pm") && stats.count("three_pa")) {
        if (stats["three_pa"] < stats["three_pm"]) {
            errors.push_back("Semantic error: three_pa cannot be less than three_pm");
        }
    }

    if (stats.count("ftm") && stats.count("fta")) {
        if (stats["fta"] < stats["ftm"]) {
            errors.push_back("Semantic error: fta cannot be less than ftm");
        }
    }
}

void SemanticAnalyzer::analyze() {
    extractAssignments();
    validateRules();
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

const std::unordered_map<std::string, int>& SemanticAnalyzer::getStats() const {
    return stats;
}
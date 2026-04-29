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
    if (text.empty()) return false;

    size_t start = 0;
    if (text[0] == '-' || text[0] == '+') {
        if (text.length() == 1) return false;
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
    for (size_t i = 0; i + 3 < tokens.size(); i++) {
        if (tokens[i].type == TokenType::IDENTIFIER &&
            tokens[i + 1].type == TokenType::ASSIGN &&
            tokens[i + 2].type == TokenType::NUMBER &&
            tokens[i + 3].type == TokenType::SEMICOLON) {

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
    // Attempt vs made checks
    if (stats.count("fgm") && stats.count("fga") && stats["fga"] < stats["fgm"]) {
        errors.push_back("Semantic error: fga cannot be less than fgm");
    }

    if (stats.count("three_pm") && stats.count("three_pa") && stats["three_pa"] < stats["three_pm"]) {
        errors.push_back("Semantic error: three_pa cannot be less than three_pm");
    }

    if (stats.count("ftm") && stats.count("fta") && stats["fta"] < stats["ftm"]) {
        errors.push_back("Semantic error: fta cannot be less than ftm");
    }

    if (stats.count("three_pm") && stats.count("fgm")) {
        if (stats["three_pm"] > stats["fgm"]) {
            errors.push_back("Semantic error: three_pm cannot be greater than fgm");
        }
    }

    // Points consistency
    if (stats.count("pts") &&
        stats.count("fgm") &&
        stats.count("three_pm") &&
        stats.count("ftm")) {

        int expected_pts = 2 * stats["fgm"] + stats["three_pm"] + stats["ftm"];

        if (stats["pts"] != expected_pts) {
            errors.push_back(
                "Semantic error: pts (" + std::to_string(stats["pts"]) +
                ") does not match expected value (" + std::to_string(expected_pts) +
                ") based on fgm, three_pm, and ftm"
            );
        }
    }
}

int SemanticAnalyzer::getTermValue(size_t index, bool& ok) {
    if (tokens[index].type == TokenType::NUMBER) {
        ok = isNumber(tokens[index].lexeme);
        return ok ? std::stoi(tokens[index].lexeme) : 0;
    }

    if (tokens[index].type == TokenType::IDENTIFIER) {
        std::string name = tokens[index].lexeme;

        if (!isValidStatName(name)) {
            errors.push_back("Line " + std::to_string(tokens[index].line) +
                             ": Invalid stat name '" + name + "' in check expression");
            ok = false;
            return 0;
        }

        // If the stat was never assigned, assume it is 0
        if (!stats.count(name)) {
            ok = true;
            return 0;
        }

        ok = true;
        return stats[name];
    }

    ok = false;
    return 0;
}

int SemanticAnalyzer::evaluateExpression(size_t startIndex, size_t endIndex, bool& ok) {
    ok = true;

    bool termOk = false;
    int value = getTermValue(startIndex, termOk);
    if (!termOk) {
        ok = false;
        return 0;
    }

    size_t i = startIndex + 1;

    while (i < endIndex) {
        TokenType op = tokens[i].type;

        if (op != TokenType::PLUS && op != TokenType::MINUS) {
            errors.push_back("Line " + std::to_string(tokens[i].line) +
                             ": Invalid operator in expression");
            ok = false;
            return 0;
        }

        if (i + 1 >= endIndex) {
            errors.push_back("Line " + std::to_string(tokens[i].line) +
                             ": Incomplete expression");
            ok = false;
            return 0;
        }

        int rhs = getTermValue(i + 1, termOk);
        if (!termOk) {
            ok = false;
            return 0;
        }

        if (op == TokenType::PLUS) {
            value += rhs;
        } else {
            value -= rhs;
        }

        i += 2;
    }

    return value;
}

bool SemanticAnalyzer::evaluateComparison(int leftValue, TokenType op, int rightValue) const {
    switch (op) {
        case TokenType::GREATER: return leftValue > rightValue;
        case TokenType::LESS: return leftValue < rightValue;
        case TokenType::GREATER_EQUAL: return leftValue >= rightValue;
        case TokenType::LESS_EQUAL: return leftValue <= rightValue;
        case TokenType::EQUAL_EQUAL: return leftValue == rightValue;
        case TokenType::BANG_EQUAL: return leftValue != rightValue;
        default: return false;
    }
}

void SemanticAnalyzer::evaluateChecks() {
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type != TokenType::CHECK) continue;

        size_t opIndex = i + 1;
        while (opIndex < tokens.size() &&
               tokens[opIndex].type != TokenType::GREATER &&
               tokens[opIndex].type != TokenType::LESS &&
               tokens[opIndex].type != TokenType::GREATER_EQUAL &&
               tokens[opIndex].type != TokenType::LESS_EQUAL &&
               tokens[opIndex].type != TokenType::EQUAL_EQUAL &&
               tokens[opIndex].type != TokenType::BANG_EQUAL) {
            opIndex++;
        }

        if (opIndex >= tokens.size() || opIndex + 1 >= tokens.size()) {
            errors.push_back("Line " + std::to_string(tokens[i].line) +
                             ": Invalid check statement");
            continue;
        }

        bool exprOk = false;
        int leftValue = evaluateExpression(i + 1, opIndex, exprOk);
        if (!exprOk) continue;

        if (tokens[opIndex + 1].type != TokenType::NUMBER || !isNumber(tokens[opIndex + 1].lexeme)) {
            errors.push_back("Line " + std::to_string(tokens[opIndex + 1].line) +
                             ": Invalid comparison number in check statement");
            continue;
        }

        int rightValue = std::stoi(tokens[opIndex + 1].lexeme);
        bool result = evaluateComparison(leftValue, tokens[opIndex].type, rightValue);

        std::string exprText;
        for (size_t j = i + 1; j <= opIndex + 1; j++) {
            if (!exprText.empty()) exprText += " ";
            exprText += tokens[j].lexeme;
        }

        checkResults.push_back(exprText + " -> " + (result ? "true" : "false"));
    }
}

void SemanticAnalyzer::analyze() {
    extractAssignments();
    validateRules();
    if (errors.empty()) {
        evaluateChecks();
    }
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

const std::vector<std::string>& SemanticAnalyzer::getCheckResults() const {
    return checkResults;
}

const std::unordered_map<std::string, int>& SemanticAnalyzer::getStats() const {
    return stats;
}
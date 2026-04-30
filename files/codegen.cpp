#include "codegen.h"

CodeGenerator::CodeGenerator(const std::vector<Token>& tokenList)
    : tokens(tokenList) {}

void CodeGenerator::generatePlayer(size_t& i) {
    std::string name = tokens[i + 1].lexeme;
    instructions.push_back("SET_PLAYER \"" + name + "\"");
    i += 2;
}

void CodeGenerator::generateTeam(size_t& i) {
    std::string team = tokens[i + 1].lexeme;
    instructions.push_back("SET_TEAM \"" + team + "\"");
    i += 2;
}
std::string CodeGenerator::relOpToInstruction(TokenType type) const {
    switch (type) {
        case TokenType::GREATER: return "CMP_GT";
        case TokenType::LESS: return "CMP_LT";
        case TokenType::GREATER_EQUAL: return "CMP_GE";
        case TokenType::LESS_EQUAL: return "CMP_LE";
        case TokenType::EQUAL_EQUAL: return "CMP_EQ";
        case TokenType::BANG_EQUAL: return "CMP_NE";
        default: return "CMP_UNKNOWN";
    }
}

void CodeGenerator::generateAssignment(size_t& i) {
    // Pattern: IDENTIFIER ASSIGN NUMBER SEMICOLON
    std::string name = tokens[i].lexeme;
    std::string value = tokens[i + 2].lexeme;

    instructions.push_back("LOAD_CONST " + value);
    instructions.push_back("STORE " + name);

    i += 3;
}

void CodeGenerator::generateCheck(size_t& i) {
    // Pattern:
    // CHECK Expr RelOp NUMBER SEMICOLON
    // where Expr is Term ((PLUS|MINUS) Term)*

    size_t j = i + 1;

    // First term
    if (tokens[j].type == TokenType::IDENTIFIER) {
        instructions.push_back("LOAD_VAR " + tokens[j].lexeme);
    } else if (tokens[j].type == TokenType::NUMBER) {
        instructions.push_back("LOAD_CONST " + tokens[j].lexeme);
    }
    j++;

    // Remaining expression terms until relational operator
    while (j < tokens.size() &&
           tokens[j].type != TokenType::GREATER &&
           tokens[j].type != TokenType::LESS &&
           tokens[j].type != TokenType::GREATER_EQUAL &&
           tokens[j].type != TokenType::LESS_EQUAL &&
           tokens[j].type != TokenType::EQUAL_EQUAL &&
           tokens[j].type != TokenType::BANG_EQUAL) {

        TokenType op = tokens[j].type;
        j++;

        if (tokens[j].type == TokenType::IDENTIFIER) {
            instructions.push_back("LOAD_VAR " + tokens[j].lexeme);
        } else if (tokens[j].type == TokenType::NUMBER) {
            instructions.push_back("LOAD_CONST " + tokens[j].lexeme);
        }

        if (op == TokenType::PLUS) {
            instructions.push_back("ADD");
        } else if (op == TokenType::MINUS) {
            instructions.push_back("SUB");
        }

        j++;
    }

    // Relational operator
    TokenType relOp = tokens[j].type;
    j++;

    // Right-hand comparison number
    instructions.push_back("LOAD_CONST " + tokens[j].lexeme);
    instructions.push_back(relOpToInstruction(relOp));
    instructions.push_back("CHECK_RESULT");

    // Move i to semicolon
    while (i < tokens.size() && tokens[i].type != TokenType::SEMICOLON) {
        i++;
    }
}

void CodeGenerator::generate() {
    for (size_t i = 0; i < tokens.size(); i++) {

        if (tokens[i].type == TokenType::PLAYER) {
            generatePlayer(i);
        }
        else if (tokens[i].type == TokenType::TEAM) {
            generateTeam(i);
        }
        else if (tokens[i].type == TokenType::IDENTIFIER &&
                 i + 3 < tokens.size() &&
                 tokens[i + 1].type == TokenType::ASSIGN &&
                 tokens[i + 2].type == TokenType::NUMBER &&
                 tokens[i + 3].type == TokenType::SEMICOLON) {

            generateAssignment(i);
        }
        else if (tokens[i].type == TokenType::CHECK) {
            generateCheck(i);
        }
    }
}

const std::vector<std::string>& CodeGenerator::getInstructions() const {
    return instructions;
}
#include "token.h"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::PLAYER: return "PLAYER";
        case TokenType::TEAM: return "TEAM";
        case TokenType::CHECK: return "CHECK";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}
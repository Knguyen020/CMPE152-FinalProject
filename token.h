#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    PLAYER,
    TEAM,

    // General token types
    IDENTIFIER,
    STRING,
    NUMBER,

    // Symbols
    ASSIGN,      // =
    SEMICOLON,   // ;

    // Special
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType t, const std::string& l, int ln)
        : type(t), lexeme(l), line(ln) {}
};

std::string tokenTypeToString(TokenType type);

#endif
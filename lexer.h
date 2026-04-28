#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"

class Lexer {
private:
    std::string source;   // entire input file as a string
    std::vector<Token> tokens;

    int start = 0;        // start of current lexeme
    int current = 0;      // current position in source
    int line = 1;         // track line numbers

    // Core scanning helpers
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;

    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);

    // Token recognition functions
    void scanToken();
    void identifier();
    void number();
    void string();

    // Utility helpers
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;

public:
    Lexer(const std::string& src);

    std::vector<Token> scanTokens();
};

#endif
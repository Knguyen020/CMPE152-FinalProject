#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"

class Lexer {
private:
    std::string source;
    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;

    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);

    void scanToken();
    void identifier();
    void number();          // normal number like 30
    void signedNumber();    // signed number like +4 or -4
    void string();

    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;

public:
    Lexer(const std::string& src);

    std::vector<Token> scanTokens();
};

#endif
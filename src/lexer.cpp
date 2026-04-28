#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& src) : source(src) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "", line));
    return tokens;
}

bool Lexer::isAtEnd() const {
    return current >= static_cast<int>(source.length());
}

char Lexer::advance() {
    return source[current++];
}

char Lexer::peek() const {
    if (isAtEnd()) {
        return '\0';
    }
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= static_cast<int>(source.length())) {
        return '\0';
    }
    return source[current + 1];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, line));
}

void Lexer::addToken(TokenType type, const std::string& lexeme) {
    tokens.push_back(Token(type, lexeme, line));
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

void Lexer::scanToken() {
    char c = advance();

    switch (c) {
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;

        case '\n':
            line++;
            break;

        case '=':
            addToken(TokenType::ASSIGN);
            break;

        case ';':
            addToken(TokenType::SEMICOLON);
            break;

        case '"':
            string();
            break;

        default:
            if (isAlpha(c)) {
                identifier();
            } else if (isDigit(c)) {
                number();
            } else {
                addToken(TokenType::INVALID, std::string(1, c));
            }
            break;
    }
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string text = source.substr(start, current - start);

    if (text == "player") {
        addToken(TokenType::PLAYER, text);
    } else if (text == "team") {
        addToken(TokenType::TEAM, text);
    } else {
        addToken(TokenType::IDENTIFIER, text);
    }
}

void Lexer::number() {
    while (isDigit(peek())) {
        advance();
    }

    std::string text = source.substr(start, current - start);
    addToken(TokenType::NUMBER, text);
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }

    if (isAtEnd()) {
        addToken(TokenType::INVALID, "Unterminated string");
        return;
    }

    // Consume closing quote
    advance();

    // Extract contents without quotes
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}
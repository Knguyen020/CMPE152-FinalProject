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
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= static_cast<int>(source.length())) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
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
            break;

        case '\n':
            line++;
            break;

        case ';':
            addToken(TokenType::SEMICOLON);
            break;

        case '"':
            string();
            break;

        case '=':
            if (match('=')) {
                addToken(TokenType::EQUAL_EQUAL);
            } else {
                addToken(TokenType::ASSIGN);
            }
            break;

        case '!':
            if (match('=')) {
                addToken(TokenType::BANG_EQUAL);
            } else {
                addToken(TokenType::INVALID, "!");
            }
            break;

        case '>':
            if (match('=')) {
                addToken(TokenType::GREATER_EQUAL);
            } else {
                addToken(TokenType::GREATER);
            }
            break;

        case '<':
            if (match('=')) {
                addToken(TokenType::LESS_EQUAL);
            } else {
                addToken(TokenType::LESS);
            }
            break;

        case '+':
            if (isDigit(peek())) {
                signedNumber();
            } else {
                addToken(TokenType::PLUS);
            }
            break;

        case '-':
            if (isDigit(peek())) {
                signedNumber();
            } else {
                addToken(TokenType::MINUS);
            }
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
    } else if (text == "check") {
        addToken(TokenType::CHECK, text);
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

void Lexer::signedNumber() {
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

    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}
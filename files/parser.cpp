#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokenList) : tokens(tokenList) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        return advance();
    }

    std::string error = "Line " + std::to_string(peek().line) + ": " + errorMessage;
    errors.push_back(error);

    // Return an invalid placeholder token so parsing can continue
    return Token(TokenType::INVALID, "", peek().line);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) {
            return;
        }

        if (peek().type == TokenType::PLAYER ||
            peek().type == TokenType::TEAM ||
            peek().type == TokenType::IDENTIFIER) {
            return;
        }

        advance();
    }
}

void Parser::playerDeclaration() {
    consume(TokenType::STRING, "Expected player name as a string after 'player'");
    consume(TokenType::SEMICOLON, "Expected ';' after player declaration");
}

void Parser::teamDeclaration() {
    consume(TokenType::STRING, "Expected team name as a string after 'team'");
    consume(TokenType::SEMICOLON, "Expected ';' after team declaration");
}

void Parser::statAssignment() {
    consume(TokenType::ASSIGN, "Expected '=' after identifier");
    consume(TokenType::NUMBER, "Expected number after '='");
    consume(TokenType::SEMICOLON, "Expected ';' after stat assignment");
}

void Parser::statement() {
    if (match(TokenType::PLAYER)) {
        playerDeclaration();
    }
    else if (match(TokenType::TEAM)) {
        teamDeclaration();
    }
    else if (match(TokenType::IDENTIFIER)) {
        statAssignment();
    }
    else {
        std::string error = "Line " + std::to_string(peek().line) +
                            ": Unexpected token '" + peek().lexeme + "'";
        errors.push_back(error);
        synchronize();
    }
}

void Parser::parse() {
    while (!isAtEnd()) {
        statement();
    }
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}
#include "parser.h"
#include <string>

Parser::Parser(const std::vector<Token>& tokenList)
    : tokens(tokenList), current(0) {}

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
    if (isAtEnd()) return false;
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
            peek().type == TokenType::CHECK ||
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

void Parser::term() {
    if (match(TokenType::IDENTIFIER) || match(TokenType::NUMBER)) {
        return;
    }

    std::string error = "Line " + std::to_string(peek().line) +
                        ": Expected identifier or number in expression";
    errors.push_back(error);
}

void Parser::expression() {
    term();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        term();
    }
}

void Parser::relationalOperator() {
    if (match(TokenType::GREATER) ||
        match(TokenType::LESS) ||
        match(TokenType::GREATER_EQUAL) ||
        match(TokenType::LESS_EQUAL) ||
        match(TokenType::EQUAL_EQUAL) ||
        match(TokenType::BANG_EQUAL)) {
        return;
    }

    std::string error = "Line " + std::to_string(peek().line) +
                        ": Expected comparison operator";
    errors.push_back(error);
}

void Parser::checkStatement() {
    expression();
    relationalOperator();
    consume(TokenType::NUMBER, "Expected number after comparison operator");
    consume(TokenType::SEMICOLON, "Expected ';' after check statement");
}

void Parser::statement() {
    if (match(TokenType::PLAYER)) {
        playerDeclaration();
    }
    else if (match(TokenType::TEAM)) {
        teamDeclaration();
    }
    else if (match(TokenType::CHECK)) {
        checkStatement();
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
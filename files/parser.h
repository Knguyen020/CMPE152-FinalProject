#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "token.h"

class Parser {
private:
    std::vector<Token> tokens;
    int current;
    std::vector<std::string> errors;

    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);

    Token consume(TokenType type, const std::string& errorMessage);
    void synchronize();

    void statement();
    void playerDeclaration();
    void teamDeclaration();
    void statAssignment();
    void checkStatement();

    void expression();
    void term();
    void relationalOperator();

public:
    Parser(const std::vector<Token>& tokenList);

    void parse();
    const std::vector<std::string>& getErrors() const;
};

#endif
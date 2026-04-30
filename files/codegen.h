#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include <string>
#include "token.h"

class CodeGenerator {
private:
    std::vector<Token> tokens;
    std::vector<std::string> instructions;

    void generateAssignment(size_t& i);
    void generateCheck(size_t& i);
    std::string relOpToInstruction(TokenType type) const;

public:
    CodeGenerator(const std::vector<Token>& tokenList);

    void generate();
    const std::vector<std::string>& getInstructions() const;
};

#endif
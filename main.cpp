#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"
#include "token.h"

std::string readFile(const std::string& filename) {
    std::ifstream inFile(filename);

    if (!inFile) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
}

int main() {
    std::string filename = "test1.txt";
    std::string source = readFile(filename);

    if (source.empty()) {
        return 1;
    }

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    for (const Token& token : tokens) {
        std::cout << "Line " << token.line
                  << " | " << tokenTypeToString(token.type)
                  << " | " << token.lexeme << std::endl;
    }

    return 0;
}

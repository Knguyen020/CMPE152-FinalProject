#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
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
    std::string filename = "testGood.txt";
    //std::string filename = "testBad.txt";
    
    std::string source = readFile(filename);

    if (source.empty()) {
        return 1;
    }

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::cout << "TOKENS:" << std::endl;
    for (const Token& token : tokens) {
        std::cout << "Line " << token.line
                  << " | " << tokenTypeToString(token.type)
                  << " | " << token.lexeme << std::endl;
    }

    std::cout << "\nPARSER RESULT:" << std::endl;
    Parser parser(tokens);
    parser.parse();

    const std::vector<std::string>& parseErrors = parser.getErrors();

    if (!parseErrors.empty()) {
        std::cout << "Syntax errors found:" << std::endl;
        for (const std::string& error : parseErrors) {
            std::cout << error << std::endl;
        }
        return 1;
    }

    std::cout << "Parsing completed successfully. No syntax errors found." << std::endl;

    std::cout << "\nSEMANTIC RESULT:" << std::endl;
    SemanticAnalyzer semanticAnalyzer(tokens);
    semanticAnalyzer.analyze();

    const std::vector<std::string>& semanticErrors = semanticAnalyzer.getErrors();

    if (!semanticErrors.empty()) {
        std::cout << "Semantic errors found:" << std::endl;
        for (const std::string& error : semanticErrors) {
            std::cout << error << std::endl;
        }
        return 1;
    }

    std::cout << "Semantic analysis completed successfully. No semantic errors found." << std::endl;

    const std::unordered_map<std::string, int>& stats = semanticAnalyzer.getStats();

    std::cout << "\nFINAL STAT LINE:\n";

    auto getStat = [&](const std::string& key) {
        return stats.count(key) ? stats.at(key) : 0;
    };

    std::cout << std::left
        << std::setw(6) << "MIN"
        << std::setw(6) << "PTS"
        << std::setw(6) << "FGM"
        << std::setw(6) << "FGA"
        << std::setw(6) << "3PM"
        << std::setw(6) << "3PA"
        << std::setw(6) << "FTM"
        << std::setw(6) << "FTA"
        << std::setw(6) << "REB"
        << std::setw(6) << "AST"
        << std::setw(6) << "STL"
        << std::setw(6) << "BLK"
        << std::setw(6) << "TO"
        << std::setw(6) << "PF"
        << std::setw(6) << "+/-"
        << std::endl;

    std::cout << std::string(90, '-') << std::endl;

    int plusMinusValue = getStat("plus_minus");
    std::string plusMinusText = (plusMinusValue > 0 ? "+" : "") + std::to_string(plusMinusValue);

    std::cout << std::left
        << std::setw(6) << getStat("min")
        << std::setw(6) << getStat("pts")
        << std::setw(6) << getStat("fgm")
        << std::setw(6) << getStat("fga")
        << std::setw(6) << getStat("three_pm")
        << std::setw(6) << getStat("three_pa")
        << std::setw(6) << getStat("ftm")
        << std::setw(6) << getStat("fta")
        << std::setw(6) << getStat("reb")
        << std::setw(6) << getStat("ast")
        << std::setw(6) << getStat("stl")
        << std::setw(6) << getStat("blk")
        << std::setw(6) << getStat("to")
        << std::setw(6) << getStat("pf")
        << std::setw(6) << plusMinusText
        << std::endl;

    const std::vector<std::string>& checkResults = semanticAnalyzer.getCheckResults();
    if (!checkResults.empty()) {
        std::cout << "\nPARLAY RESULTS:\n";
        for (const std::string& result : checkResults) {
            std::cout << result << std::endl;
        }
    }

    std::cout << "\nINTERMEDIATE CODE:\n";
    CodeGenerator codeGenerator(tokens);
    codeGenerator.generate();

    const std::vector<std::string>& instructions = codeGenerator.getInstructions();
    for (const std::string& instruction : instructions) {
        std::cout << instruction << std::endl;
    }

    return 0;
}
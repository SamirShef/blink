#include "codegen/codegen.hpp"
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string token_to_string(Token& token);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Use: cc <source_name>\n";
        return 1;
    }
    
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error opening file!\n";
        return 1;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Lexer lexer(content);
    std::vector<Token> tokens = lexer.tokenize();

    for (Token token : tokens) {
        std::cout << token_to_string(token) << '\n';
    }

    std::cout << "PARSING\n";

    Parser parser(tokens);
    std::vector<StmtPtr> stmts = parser.parse();

    std::cout << "CODE GENERATING\n";
    CodeGenerator codegen("cc", std::move(stmts));
    codegen.generate();

    return 0;
}

std::string token_to_string(Token& token) {
    return "'" + std::to_string((int)token.type) + "' : '" + token.value + "' (" + std::to_string(token.line) + ":" + std::to_string(token.column) + ")";
}
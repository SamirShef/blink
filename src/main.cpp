#include "../include/lexer/lexer.hpp"
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

    return 0;
}

std::string token_type_to_string(TokenType& type) {
    switch (type) {
        case TokenType::SIGNED:
            return "SIGNED";
        case TokenType::UNSIGNED:
            return "UNSIGNED";
        case TokenType::CHAR:
            return "CHAR";
        case TokenType::SHORT:
            return "SHORT";
        case TokenType::INT:
            return "INT";
        case TokenType::LONG:
            return "LONG";
        case TokenType::FLOAT:
            return "FLOAT";
        case TokenType::DOUBLE:
            return "DOUBLE";
        case TokenType::VOID:
            return "VOID";
        case TokenType::TYPEDEF:
            return "TYPEDEF";
        case TokenType::STRUCT:
            return "STRUCT";
        case TokenType::SIZEOF:
            return "SIZEOF";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::DO:
            return "DO";
        case TokenType::FOR:
            return "FOR";
        case TokenType::BREAK:
            return "BREAK";
        case TokenType::CONTINUE:
            return "CONTINUE";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::LPAREN:
            return "LPAREN";
        case TokenType::RPAREN:
            return "RPAREN";
        case TokenType::LBRACKET:
            return "LBRACKET";
        case TokenType::RBRACKET:
            return "RBRACKET";
        case TokenType::LBRACE:
            return "LBRACE";
        case TokenType::RBRACE:
            return "RBRACE";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::COLON:
            return "COLON";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::MULT:
            return "MULT";
        case TokenType::DIV:
            return "DIV";
        case TokenType::MODULO:
            return "MODULO";
        case TokenType::PLUS_EQ:
            return "PLUS_EQ";
        case TokenType::MINUS_EQ:
            return "MINUS_EQ";
        case TokenType::MULT_EQ:
            return "MULT_EQ";
        case TokenType::DIV_EQ:
            return "DIV_EQ";
        case TokenType::MODULO_EQ:
            return "MODULO_EQ";
        case TokenType::EQ:
            return "EQ";
        case TokenType::EQ_EQ:
            return "EQ_EQ";
        case TokenType::NOT_EQ:
            return "NOT_EQ";
        case TokenType::GT:
            return "GT";
        case TokenType::GT_EQ:
            return "GT_EQ";
        case TokenType::LS:
            return "LS";
        case TokenType::LS_EQ:
            return "LS_EQ";
        case TokenType::L_AND:
            return "L_AND";
        case TokenType::L_OR:
            return "L_OR";
        case TokenType::CHAR_LIT:
            return "CHAR_LIT";
        case TokenType::SHORT_LIT:
            return "SHORT_LIT";
        case TokenType::INT_LIT:
            return "INT_LIT";
        case TokenType::LONG_LIT:
            return "LONG_LIT";
        case TokenType::FLOAT_LIT:
            return "FLOAT_LIT";
        case TokenType::DOUBLE_LIT:
            return "DOUBLE_LIT";
        case TokenType::STRING_LIT:
            return "STRING_LIT";
        case TokenType::ID:
            return "ID";
        default:
            return std::to_string((int)type);
    }
}

std::string token_to_string(Token& token) {
    return "'" + token_type_to_string(token.type) + "' : '" + token.value + "' (" + std::to_string(token.line) + ":" + std::to_string(token.column) + ")";
}
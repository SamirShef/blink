#include "../../include/parser/parser.hpp"
#include "../../include/lexer/token.hpp"
#include "../../include/parser/ast.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> stmts;

    while (pos < tokens_len) {
        stmts.push_back(parse_stmt());
    }
    
    return stmts;
}

StmtPtr Parser::parse_stmt() {
    if (peek().type == TokenType::CONST || peek().type == TokenType::UNSIGNED || peek().type == TokenType::SIGNED || is_type(peek().type)) {
        int tmp_pos = pos;
        consume_type();
        if (peek(1).type == TokenType::LPAREN) {
            pos = tmp_pos;
            return parse_func_decl_stmt();
        }
        else {
            pos = tmp_pos;
            return parse_var_decl_stmt();
        }
    }
    else {
        std::cerr << "Unsupported token '" << peek().value << "'\n";
        exit(1);
    }
}

StmtPtr Parser::parse_var_decl_stmt() {
    Type var_type = consume_type();
    std::string var_name = consume(TokenType::ID, "Expected identifier", peek().line, peek().column).value;

    ExprPtr var_expr = nullptr;
    if (match(TokenType::EQ)) {
        var_expr = parse_expr();
    }

    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    
    return std::make_unique<VarDeclStmt>(var_type, var_name, std::move(var_expr));
}

StmtPtr Parser::parse_func_decl_stmt() {
    Type var_type = consume_type();
    std::string var_name = consume(TokenType::ID, "Expected identifier", peek().line, peek().column).value;
    consume(TokenType::LPAREN, "Expected '('", peek().line, peek().column);
    std::vector<Argument> args;
    while (!match(TokenType::RPAREN)) {
        args.push_back(parse_argument());
    }
    consume(TokenType::LBRACE, "Expected '{'", peek().line, peek().column);
    
    std::vector<StmtPtr> block;
    while (!match(TokenType::RBRACE)) {
        block.push_back(parse_stmt());
    }
    
    return std::make_unique<FuncDeclStmt>(var_type, var_name, std::move(args), std::move(block));
}

Argument Parser::parse_argument() {
    Type arg_type = consume_type();
    std::string arg_name = consume(TokenType::ID, "Expected identifier", peek().line, peek().column).value;
    ExprPtr arg_expr = nullptr;
    if (match(TokenType::EQ)) {
        arg_expr = parse_expr();
    }
    if (peek().type != TokenType::RPAREN) {
        consume(TokenType::COMMA, "Expected ','", peek().line, peek().column);
    }
    return Argument(arg_type, arg_name, std::move(arg_expr));
}

ExprPtr Parser::parse_expr() {
    return parse_primary();
}

ExprPtr Parser::parse_l_or() {

}

ExprPtr Parser::parse_l_and() {

}

ExprPtr Parser::parse_equality() {

}

ExprPtr Parser::parse_comparation() {

}

ExprPtr Parser::parse_multiplicative() {

}

ExprPtr Parser::parse_additive() {

}

ExprPtr Parser::parse_unary() {

}

ExprPtr Parser::parse_primary() {
    Token token = peek();

    switch (token.type) {
        case TokenType::CHAR_LIT:
            pos++;
            return std::make_unique<CharLiteral>(std::stoll(token.value));
        case TokenType::SHORT_LIT:
            pos++;
            return std::make_unique<ShortLiteral>(std::stoll(token.value));
        case TokenType::INT_LIT:
            pos++;
            return std::make_unique<IntLiteral>(std::stoll(token.value));
        case TokenType::LONG_LIT:
            pos++;
            return std::make_unique<LongLiteral>(std::stoll(token.value));
        case TokenType::FLOAT_LIT:
            pos++;
            return std::make_unique<FloatLiteral>(std::stold(token.value));
        case TokenType::DOUBLE_LIT:
            pos++;
            return std::make_unique<DoubleLiteral>(std::stold(token.value));
        default:
            std::cerr << "Unexpected token '" << token.value << "'\n";
            exit(1);
    }
}

bool Parser::is_type(TokenType type) const {
    return type == TokenType::CHAR || type == TokenType::SHORT || type == TokenType::INT
        || type == TokenType::LONG || type == TokenType::FLOAT || type == TokenType::DOUBLE;
}

TypeValue Parser::token_type_to_type_value(Token token) const {
    if (is_type(token.type)) {
        switch (token.type) {
            case TokenType::CHAR:
                return TypeValue::CHAR;
            case TokenType::SHORT:
                return TypeValue::SHORT;
            case TokenType::INT:
                return TypeValue::INT;
            case TokenType::LONG:
                return TypeValue::LONG;
            case TokenType::FLOAT:
                return TypeValue::FLOAT;
            case TokenType::DOUBLE:
                return TypeValue::DOUBLE;
        }
    }
    else if (token.type == TokenType::STRUCT) {
        return TypeValue::STRUCT;
    }
    else if (token.type == TokenType::ENUM) {
        return TypeValue::ENUM;
    }
    else {
        std::cerr << "Expected type " << '(' << token.line << ':' << token.column << ')' << '\n';
        exit(1);
    }
}

Type Parser::consume_type() {
    bool is_const = match(TokenType::CONST);
    bool is_unsigned = false;
    if (match(TokenType::UNSIGNED)) {
        is_unsigned = true;
    }
    else if (match(TokenType::SIGNED)) {}
    TypeSpecifier specifier = TypeSpecifier::NONE;
    if ((peek(1).type != TokenType::ID && peek(1).type != TokenType::MULT) && (peek().type == TokenType::LONG || peek().type == TokenType::SHORT)) {
        if (peek().type == TokenType::LONG) {
            specifier = TypeSpecifier::LONG;
        }
        else {
            specifier = TypeSpecifier::SHORT;
        }
        pos++;
    }
    Token token = peek();
    if (!is_type(token.type)) {
        std::cerr << "Expected type " << '(' << token.line << ':' << token.column << ')' << '\n';
        exit(1);
    }
    pos++;
    bool is_pointer = match(TokenType::MULT);
    return Type(token_type_to_type_value(token), token.value, specifier, is_const, is_unsigned, is_pointer);
}

Token Parser::peek(int rpos) const {
    if (pos + rpos >= tokens_len) {
        std::cerr << "Index out of range: (" << pos + rpos << "/" << tokens_len << ")\n";
        exit(1);
    }
    return tokens[pos + rpos];
}

Token Parser::consume(TokenType type, std::string err_msg, int line, int column) {
    Token token = peek();
    if (token.type == type) {
        pos++;
        return token;
    }
    std::cerr << err_msg << " (" << line << ':' << column << ')' << '\n';
    exit(1);
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        pos++;
        return true;
    }
    return false;
}
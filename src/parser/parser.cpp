#include <parser/parser.hpp>
#include <lexer/token.hpp>
#include <parser/ast.hpp>
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
        return parse_var_decl_stmt();
    }
    else {
        std::cerr << "Unsupported token '" + peek().value + "'\n";
        exit(1);
    }
}

StmtPtr Parser::parse_var_decl_stmt() {
    bool is_const = match(TokenType::CONST);
    bool is_unsigned = false;
    if (match(TokenType::UNSIGNED)) {
        is_unsigned = true;
    }
    else if (match(TokenType::SIGNED)) {}
    TypeSpecifier specifier = TypeSpecifier::NONE;
    if (peek(1).type != TokenType::ID && (peek().type == TokenType::LONG || peek().type == TokenType::SHORT)) {
        if (peek().type == TokenType::LONG) {
            specifier = TypeSpecifier::LONG;
        }
        else {
            specifier = TypeSpecifier::SHORT;
        }
        pos++;
    }

    Type var_type = consume_type(specifier, is_const, is_unsigned);
    std::string var_name = consume(TokenType::ID, "Expected identifier").value;

    ExprPtr var_expr = nullptr;
    if (match(TokenType::EQ)) {
        var_expr = parse_expr();
    }

    consume(TokenType::SEMICOLON, "Expected ';'");

    std::cout << "Var decl: " << var_name << "(" << (int)var_type.type << "&" << (var_type.is_const ? "c_" : "") << (var_type.is_unsigned ? "u_" : "s_")
              << var_type.name << (var_type.is_pointer ? "*" : "") << ") = " << var_expr << '\n';
    
    return std::make_unique<VarDeclStmt>(var_type, var_name, std::move(var_expr));
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
            std::cerr << "Unexpected token '" + token.value + "'\n";
            exit(1);
    }
}

bool Parser::is_type(TokenType type) const {
    return type == TokenType::CHAR || type == TokenType::SHORT || type == TokenType::INT
        || type == TokenType::LONG || type == TokenType::FLOAT || type == TokenType::DOUBLE;
}

TypeValue Parser::token_type_to_type_value(TokenType type) const {
    if (is_type(type)) {
        switch (type) {
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
    else if (type == TokenType::STRUCT) {
        return TypeValue::STRUCT;
    }
    else if (type == TokenType::ENUM) {
        return TypeValue::ENUM;
    }
    else {
        std::cerr << "Expected type\n";
        exit(1);
    }
}

Type Parser::consume_type(TypeSpecifier specifier, bool is_const, bool is_unsigned) {
    Token token = peek();
    if (!is_type(token.type)) {
        std::cerr << "Expected type\n";
        exit(1);
    }
    pos++;
    bool is_pointer = match(TokenType::MULT);
    return Type(token_type_to_type_value(token.type), token.value, specifier, is_const, is_unsigned, is_pointer);
}

Token Parser::peek(int rpos) const {
    if (pos + rpos >= tokens_len) {
        std::cerr << "Index out of range: (" + std::to_string(pos + rpos) + "/" + std::to_string(tokens_len) + ")\n";
        exit(1);
    }
    return tokens[pos + rpos];
}

Token Parser::consume(TokenType type, std::string err_msg) {
    Token token = peek();
    if (token.type == type) {
        pos++;
        return token;
    }
    std::cerr << err_msg << '\n';
    exit(1);
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        pos++;
        return true;
    }
    return false;
}
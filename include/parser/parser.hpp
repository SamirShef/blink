#pragma once
#include "../lexer/token.hpp"
#include "ast.hpp"
#include <string>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    unsigned long tokens_len;
    int pos;

public:
    Parser(std::vector<Token> t) : tokens(t), tokens_len(t.size()), pos(0) {}

    std::vector<StmtPtr> parse();
private:
    StmtPtr parse_stmt();
    StmtPtr parse_var_decl_stmt();
    StmtPtr parse_func_decl_stmt();

    Argument parse_argument();

    ExprPtr parse_expr();
    ExprPtr parse_l_or();
    ExprPtr parse_l_and();
    ExprPtr parse_equality();
    ExprPtr parse_comparation();
    ExprPtr parse_multiplicative();
    ExprPtr parse_additive();
    ExprPtr parse_unary();
    ExprPtr parse_primary();

    bool is_type(TokenType type) const;
    TypeValue token_type_to_type_value(Token token) const;
    Type consume_type();
    
    Token peek(int rpos = 0) const;
    Token consume(TokenType type, std::string err_msg, int line, int column);
    bool match(TokenType type);
};
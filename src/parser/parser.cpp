#include "../../include/exception/exception.hpp"
#include "../../include/parser/parser.hpp"
#include "../../include/lexer/token.hpp"
#include "../../include/parser/ast.hpp"

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> stmts;

    while (pos < tokens_len) {
        stmts.push_back(parse_stmt());
    }
    
    return stmts;
}

StmtPtr Parser::parse_stmt() {
    if (peek().type == TokenType::CONST || peek().type == TokenType::VAR) {
        return parse_var_decl_stmt();
    }
    else if (match(TokenType::FUNC)) {
        return parse_func_decl_stmt();
    }
    else if (peek().type == TokenType::ID) {
        if (peek(1).type == TokenType::LPAREN) {
            return parse_func_call_stmt();
        }
        else {
            return parse_var_asgn_stmt();
        }
    }
    else if (match(TokenType::IF)) {
        return parse_if_stmt();
    }
    else if (match(TokenType::FOR)) {
        return parse_for_cycle_stmt();
    }
    else if (match(TokenType::WHILE)) {
        return parse_while_cycle_stmt();
    }
    else if (match(TokenType::DO)) {
        return parse_do_while_cycle_stmt();
    }
    else if (match(TokenType::BREAK)) {
        return parse_break_stmt();
    }
    else if (match(TokenType::CONTINUE)) {
        return parse_continue_stmt();
    }
    else if (match(TokenType::RETURN)) {
        return parse_return_stmt();
    }
    else {
        throw_error(peek().file_name, PARSER, "Unsupported token '" + peek().value + "' (" + std::to_string(peek().line) + ':' + std::to_string(peek().column) + ")\n", peek().line);
    }
}

StmtPtr Parser::parse_var_decl_stmt() {
    bool is_const = false;
    if (match(TokenType::CONST)) {
        is_const = true;
    }
    else if (match(TokenType::VAR)) {}
    Token var_keyword = peek(-1);
    std::string var_name = consume(TokenType::ID, "Expected identifier", peek().line, peek().column).value;
    consume(TokenType::COLON, "Expected ':'", peek().line, peek().column);
    Type var_type = consume_type(is_const);

    ExprPtr var_expr = nullptr;
    if (match(TokenType::EQ)) {
        var_expr = parse_expr();
    }

    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    
    return std::make_unique<VarDeclStmt>(var_type, var_name, std::move(var_expr), var_keyword);
}

StmtPtr Parser::parse_func_decl_stmt() {
    Token func_name_token = consume(TokenType::ID, "Expected identifier", peek().line, peek().column);
    std::string func_name = func_name_token.value;
    consume(TokenType::LPAREN, "Expected '('", peek().line, peek().column);
    std::vector<Argument> args;
    while (!match(TokenType::RPAREN)) {
        args.push_back(parse_argument());
    }
    consume(TokenType::COLON, "Expected ':'", peek().line, peek().column);
    bool is_const = false;
    if (match(TokenType::CONST)) {
        is_const = true;
    }
    Type func_type = consume_type(is_const);
    consume(TokenType::LBRACE, "Expected '{'", peek().line, peek().column);
    
    std::vector<StmtPtr> block;
    while (!match(TokenType::RBRACE)) {
        block.push_back(parse_stmt());
    }
    
    return std::make_unique<FuncDeclStmt>(func_type, func_name, std::move(args), std::move(block), func_name_token);
}

StmtPtr Parser::parse_func_call_stmt() {
    Token func_name_token = consume(TokenType::ID, "Expected identifier", peek().line, peek().column);
    std::string func_name = func_name_token.value;
    pos++;
    std::vector<ExprPtr> func_args;
    while (!match(TokenType::RPAREN)) {
        func_args.push_back(parse_expr());
        if (peek().type != TokenType::RPAREN) {
            consume(TokenType::COMMA, "Expected ','", peek().line, peek().column);
        }
    }
    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    return std::make_unique<FuncCallStmt>(func_name, std::move(func_args), func_name_token);
}

StmtPtr Parser::parse_var_asgn_stmt(bool from_for_cycle) {
    Token var_name_token = consume(TokenType::ID, "Expected identifier", peek().line, peek().column);
    std::string var_name = var_name_token.value;

    Token op = peek();
    ExprPtr expr = nullptr;
    if (is_compound_assignment_operator(op.type)) {
        expr = create_compound_assignment_operator(var_name);
    }
    else {
        consume(TokenType::EQ, "Expected '='", op.line, op.column);
        expr = parse_expr();
    }
    if (!from_for_cycle) {
        consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    }
    return std::make_unique<VarAsgnStmt>(var_name, std::move(expr), var_name_token);
}

StmtPtr Parser::parse_if_stmt() {
    Token if_keyword = peek(-1);
    consume(TokenType::LPAREN, "Expected '('", peek().line, peek().column);
    ExprPtr condition = parse_expr();
    consume(TokenType::RPAREN, "Expected ')'", peek().line, peek().column);

    std::vector<StmtPtr> true_block;
    if (!match(TokenType::LBRACE)) {
        true_block.push_back(parse_stmt());
    }
    else {
        while (!match(TokenType::RBRACE)) {
            true_block.push_back(parse_stmt());
        }
    }
    std::vector<StmtPtr> false_block;
    if (match(TokenType::ELSE)) {
        if (!match(TokenType::LBRACE)) {
            false_block.push_back(parse_stmt());
        }
        else {
            while (!match(TokenType::RBRACE)) {
                false_block.push_back(parse_stmt());
            }
        }
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(true_block), std::move(false_block), if_keyword);
}

StmtPtr Parser::parse_for_cycle_stmt() {
    Token for_keyword = peek(-1);
    consume(TokenType::LPAREN, "Expected '('", peek().line, peek().column);
    StmtPtr indexator = nullptr;
    if (peek(1).type == TokenType::COLON) {
        indexator = parse_var_decl_stmt();
    }
    else {
        indexator = parse_var_asgn_stmt();
    }
    // `;` already missed before
    ExprPtr condition = parse_expr();
    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    StmtPtr iteration = parse_var_asgn_stmt(true);
    consume(TokenType::RPAREN, "Expected ')'", peek().line, peek().column);

    std::vector<StmtPtr> block;
    if (!match(TokenType::LBRACE)) {
        block.push_back(parse_stmt());
    }
    else {
        while (!match(TokenType::RBRACE)) {
            block.push_back(parse_stmt());
        }
    }

    return std::make_unique<ForCycleStmt>(std::move(indexator), std::move(condition), std::move(iteration), std::move(block), for_keyword);
}

StmtPtr Parser::parse_while_cycle_stmt() {
    Token while_keyword = peek(-1);
    consume(TokenType::LPAREN, "Expected '('", peek().line, peek().column);
    ExprPtr condition = parse_expr();
    consume(TokenType::RPAREN, "Expected ')'", peek().line, peek().column);

    std::vector<StmtPtr> block;
    if (!match(TokenType::LBRACE)) {
        block.push_back(parse_stmt());
    }
    else {
        while (!match(TokenType::RBRACE)) {
            block.push_back(parse_stmt());
        }
    }

    return std::make_unique<WhileCycleStmt>(std::move(condition), std::move(block), while_keyword);
}

StmtPtr Parser::parse_do_while_cycle_stmt() {
    Token do_keyword = peek(-1);
    std::vector<StmtPtr> block;
    if (!match(TokenType::LBRACE)) {
        block.push_back(parse_stmt());
    }
    else {
        while (!match(TokenType::RBRACE)) {
            block.push_back(parse_stmt());
        }
    }
    
    consume(TokenType::WHILE, "Expected 'while'", peek().line, peek().column);
    consume(TokenType::LPAREN, "Expected '('", peek().line, peek().column);
    ExprPtr condition = parse_expr();
    consume(TokenType::RPAREN, "Expected ')'", peek().line, peek().column);
    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);

    return std::make_unique<DoWhileCycleStmt>(std::move(condition), std::move(block), do_keyword);
}

StmtPtr Parser::parse_break_stmt() {
    Token break_keyword = peek(-1);
    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    return std::make_unique<BreakStmt>(break_keyword);
}

StmtPtr Parser::parse_continue_stmt() {
    Token continue_keyword = peek(-1);
    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    return std::make_unique<ContinueStmt>(continue_keyword);
}

StmtPtr Parser::parse_return_stmt() {
    Token return_keyword = peek(-1);
    ExprPtr expr = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        expr = parse_expr();
    }
    consume(TokenType::SEMICOLON, "Expected ';'", peek().line, peek().column);
    return std::make_unique<ReturnStmt>(std::move(expr), return_keyword);
}

Argument Parser::parse_argument() {
    Token arg_name_token = consume(TokenType::ID, "Expected identifier", peek().line, peek().column);
    std::string arg_name = arg_name_token.value;
    consume(TokenType::COLON, "Expected ':'", peek().line, peek().column);
    bool is_const = match(TokenType::CONST);
    Type arg_type = consume_type(is_const);
    ExprPtr arg_expr = nullptr;
    if (match(TokenType::EQ)) {
        arg_expr = parse_expr();
    }
    if (peek().type != TokenType::RPAREN) {
        consume(TokenType::COMMA, "Expected ','", peek().line, peek().column);
    }
    return Argument(arg_type, arg_name, std::move(arg_expr), arg_name_token);
}

ExprPtr Parser::parse_expr() {
    return parse_l_and();
}

ExprPtr Parser::parse_l_and() {
    ExprPtr expr = parse_l_or();
    
    while (match(TokenType::L_AND)) {
        expr = std::make_unique<BinaryExpr>(TokenType::L_AND, std::move(expr), std::move(parse_l_or()), expr->first_token);
    }

    return expr;
}

ExprPtr Parser::parse_l_or() {
    ExprPtr expr = parse_equality();
    
    while (match(TokenType::L_OR)) {
        expr = std::make_unique<BinaryExpr>(TokenType::L_OR, std::move(expr), std::move(parse_equality()), expr->first_token);
    }

    return expr;
}

ExprPtr Parser::parse_equality() {
    ExprPtr expr = parse_comparation();
    
    while (1) {
        if (match(TokenType::EQ_EQ)) {
            expr = std::make_unique<BinaryExpr>(TokenType::EQ_EQ, std::move(expr), std::move(parse_comparation()), expr->first_token);
        }
        else if (match(TokenType::NOT_EQ)) {
            expr = std::make_unique<BinaryExpr>(TokenType::NOT_EQ, std::move(expr), std::move(parse_comparation()), expr->first_token);
        }
        else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::parse_comparation() {
    ExprPtr expr = parse_multiplicative();
    
    while (1) {
        if (match(TokenType::GT)) {
            expr = std::make_unique<BinaryExpr>(TokenType::GT, std::move(expr), std::move(parse_multiplicative()), expr->first_token);
        }
        else if (match(TokenType::GT_EQ)) {
            expr = std::make_unique<BinaryExpr>(TokenType::GT_EQ, std::move(expr), std::move(parse_multiplicative()), expr->first_token);
        }
        else if (match(TokenType::LS)) {
            expr = std::make_unique<BinaryExpr>(TokenType::LS, std::move(expr), std::move(parse_multiplicative()), expr->first_token);
        }
        else if (match(TokenType::LS_EQ)) {
            expr = std::make_unique<BinaryExpr>(TokenType::LS_EQ, std::move(expr), std::move(parse_multiplicative()), expr->first_token);
        }
        else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::parse_multiplicative() {
    ExprPtr expr = parse_additive();
    
    while (1) {
        if (match(TokenType::MULT)) {
            expr = std::make_unique<BinaryExpr>(TokenType::MULT, std::move(expr), std::move(parse_additive()), expr->first_token);
        }
        else if (match(TokenType::DIV)) {
            expr = std::make_unique<BinaryExpr>(TokenType::DIV, std::move(expr), std::move(parse_additive()), expr->first_token);
        }
        else if (match(TokenType::MODULO)) {
            expr = std::make_unique<BinaryExpr>(TokenType::MODULO, std::move(expr), std::move(parse_additive()), expr->first_token);
        }
        else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::parse_additive() {
    ExprPtr expr = parse_unary();
    
    while (1) {
        if (match(TokenType::MINUS)) {
            expr = std::make_unique<BinaryExpr>(TokenType::MINUS, std::move(expr), std::move(parse_unary()), expr->first_token);
        }
        else if (match(TokenType::PLUS)) {
            expr = std::make_unique<BinaryExpr>(TokenType::PLUS, std::move(expr), std::move(parse_unary()), expr->first_token);
        }
        else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::parse_unary() {
    while (1) {
        if (match(TokenType::MINUS)) {
            return std::make_unique<UnaryExpr>(TokenType::MINUS, std::move(parse_primary()), peek());
        }
        else if (match(TokenType::L_NOT)) {
            return std::make_unique<UnaryExpr>(TokenType::L_NOT, std::move(parse_primary()), peek());
        }
        else {
            break;
        }
    }

    return parse_primary();
}

ExprPtr Parser::parse_primary() {
    Token token = peek();

    switch (token.type) {
        case TokenType::I8_LIT:
            pos++;
            return std::make_unique<I8Literal>(token.value[0], token);
        case TokenType::I16_LIT:
            pos++;
            return std::make_unique<I16Literal>(std::stoll(token.value), token);
        case TokenType::I32_LIT:
            pos++;
            return std::make_unique<I32Literal>(std::stoll(token.value), token);
        case TokenType::I64_LIT:
            pos++;
            return std::make_unique<I64Literal>(std::stoll(token.value), token);
        case TokenType::F32_LIT:
            pos++;
            return std::make_unique<F32Literal>(std::stold(token.value), token);
        case TokenType::F64_LIT:
            pos++;
            return std::make_unique<F64Literal>(std::stold(token.value), token);
        case TokenType::U8_LIT:
            pos++;
            return std::make_unique<U8Literal>((std::uint8_t)(token.value[0]), token);
        case TokenType::U16_LIT:
            pos++;
            return std::make_unique<U16Literal>((std::uint16_t)std::stoull(token.value), token);
        case TokenType::U32_LIT:
            pos++;
            return std::make_unique<U32Literal>((std::uint32_t)std::stoull(token.value), token);
        case TokenType::U64_LIT:
            pos++;
            return std::make_unique<U64Literal>((std::uint64_t)std::stoull(token.value), token);
        case TokenType::BOOL_LIT:
            pos++;
            return std::make_unique<BoolLiteral>(token.value == "true", token);
        case TokenType::STRING_LIT:
            pos++;
            return std::make_unique<StringLiteral>(token.value, token);
        case TokenType::ID:
            pos++;
            if (match(TokenType::LPAREN)) {
                std::vector<ExprPtr> func_args;
                while (!match(TokenType::RPAREN)) {
                    func_args.push_back(parse_expr());
                    if (peek().type != TokenType::RPAREN) {
                        consume(TokenType::COMMA, "Expected ','", peek().line, peek().column);
                    }
                }
                return std::make_unique<FuncCallExpr>(token.value, std::move(func_args), token);
            }
            return std::make_unique<VarExpr>(token.value, token);
        default:
            throw_error(token.file_name, PARSER, "Unexpected token '" + token.value + "' (" + std::to_string(token.line) + ':' + std::to_string(token.column) + ")\n", token.line);
    }
}

bool Parser::is_compound_assignment_operator(TokenType type) const {
    return type == TokenType::PLUS_EQ || type == TokenType::MINUS_EQ || type == TokenType::MULT_EQ || type == TokenType::DIV_EQ || type == TokenType::MODULO_EQ;
}

ExprPtr Parser::create_compound_assignment_operator(std::string id) {
    Token token = peek();
    pos++;
    switch (token.type) {
        case TokenType::PLUS_EQ:
            return std::make_unique<BinaryExpr>(TokenType::PLUS, std::make_unique<VarExpr>(id, token), parse_expr(), token);
        case TokenType::MINUS_EQ:
            return std::make_unique<BinaryExpr>(TokenType::MINUS, std::make_unique<VarExpr>(id, token), parse_expr(), token);
        case TokenType::MULT_EQ:
            return std::make_unique<BinaryExpr>(TokenType::MULT, std::make_unique<VarExpr>(id, token), parse_expr(), token);
        case TokenType::DIV_EQ:
            return std::make_unique<BinaryExpr>(TokenType::DIV, std::make_unique<VarExpr>(id, token), parse_expr(), token);
        case TokenType::MODULO_EQ:
            return std::make_unique<BinaryExpr>(TokenType::MODULO, std::make_unique<VarExpr>(id, token), parse_expr(), token);
        default: {
            throw_error(token.file_name, PARSER, "Unsupported compound assignment operator (" + std::to_string(token.line) + ':' + std::to_string(token.column) + ")\n", token.line);
        }
    }
}

bool Parser::is_type(TokenType type) const {
    return type == TokenType::I8 || type == TokenType::I16 || type == TokenType::I32 || type == TokenType::I64 || type == TokenType::F32 || type == TokenType::F64
        || type == TokenType::U8 || type == TokenType::U16 || type == TokenType::U32 || type == TokenType::U64 || type == TokenType::BOOL
        || type == TokenType::NOTHING;
}

bool Parser::is_unsigned_type(TokenType type) const {
    return type == TokenType::U8 || type == TokenType::U16 || type == TokenType::U32 || type == TokenType::U64;
}

TypeValue Parser::token_type_to_type_value(Token token) {
    if (is_type(token.type)) {
        switch (token.type) {
            case TokenType::I8:
                return TypeValue::I8;
            case TokenType::I16:
                return TypeValue::I16;
            case TokenType::I32:
                return TypeValue::I32;
            case TokenType::I64:
                return TypeValue::I64;
            case TokenType::F32:
                return TypeValue::F32;
            case TokenType::F64:
                return TypeValue::F64;
            case TokenType::U8:
                return TypeValue::U8;
            case TokenType::U16:
                return TypeValue::U16;
            case TokenType::U32:
                return TypeValue::U32;
            case TokenType::U64:
                return TypeValue::U64;
            case TokenType::BOOL:
                return TypeValue::BOOL;
            case TokenType::NOTHING:
                return TypeValue::NOTHING;
            default: {}
        }
    }
    else if (token.type == TokenType::CLASS) {
        return TypeValue::CLASS;
    }
    else if (token.type == TokenType::ENUM) {
        return TypeValue::ENUM;
    }
    else {
        throw_error(token.file_name, PARSER, "Expected type (" + std::to_string(token.line) + ':' + std::to_string(token.column) + ")\n", token.line);
    }
}

Type Parser::consume_type(bool is_const) {
    Token token = peek();
    if (!is_type(token.type)) {
        throw_error(token.file_name, PARSER, "Expected type (" + std::to_string(token.line) + ':' + std::to_string(token.column) + ")\n", token.line);
    }
    pos++;
    bool is_pointer = match(TokenType::MULT);
    return Type(token_type_to_type_value(token), token.value, is_const, is_unsigned_type(token.type), is_pointer);
}

Token Parser::peek(int rpos) const {
    if (pos + rpos >= tokens_len) {
        throw_error(tokens[0].file_name, PARSER, "Index out of range: (" + std::to_string(pos + rpos) + "/" + std::to_string(tokens_len) + ")\n", peek().line);
    }
    return tokens[pos + rpos];
}

Token Parser::consume(TokenType type, std::string err_msg, int line, int column) {
    Token token = peek();
    if (token.type == type) {
        pos++;
        return token;
    }
    throw_error(token.file_name, PARSER, err_msg + " (" + std::to_string(line) + ':' + std::to_string(column) + ")\n", line);
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        pos++;
        return true;
    }
    return false;
}
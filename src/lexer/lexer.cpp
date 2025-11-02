#include "../../include/exception/exception.hpp"
#include "../../include/lexer/lexer.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

std::vector<std::string> Lexer::included_files_paths;

std::map<std::string, TokenType> keywords = {
    {"i8", TokenType::I8},
    {"i16", TokenType::I16},
    {"i32", TokenType::I32},
    {"i64", TokenType::I64},
    {"f32", TokenType::F32},
    {"f64", TokenType::F64},
    {"u8", TokenType::U8},
    {"u16", TokenType::U16},
    {"u32", TokenType::U32},
    {"u64", TokenType::U64},
    {"bool", TokenType::BOOL},
    {"nothing", TokenType::NOTHING},

    {"var", TokenType::VAR},
    {"const", TokenType::CONST},
    {"func", TokenType::FUNC},
    {"class", TokenType::CLASS},
    {"enum", TokenType::ENUM},
    {"sizeof", TokenType::SIZEOF},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"do", TokenType::DO},
    {"for", TokenType::FOR},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"return", TokenType::RETURN},
};

std::vector<Token> Lexer::tokenize() {
    while (pos < source_len) {
        const char c = peek();
        if (c == '$') {
            advance();
            handle_preprocessor();
        }
        else if (c == '\n' || c == ' ') {
            advance();
        }
        else if (c == '/') {
            if (peek(1) == '/') {
                skip_singleline_comment();
            }
            else if (peek(1) == '*') {
                skip_multiline_comment();
            }
            else {
                tokens.push_back(tokenize_op());
            }
        }
        else if (std::isdigit(c)) {
            tokens.push_back(tokenize_number());
        }
        else if (c == '"') {
            tokens.push_back(tokenize_string());
        }
        else if (c == '\'') {
            tokens.push_back(tokenize_char());
        }
        else if (std::isalpha(c) || c == '_') {
            tokens.push_back(tokenize_id_or_keyword());
        }
        else {
            tokens.push_back(tokenize_op());
        }
    }

    return tokens;
}

Token Lexer::tokenize_number() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;
    bool has_dot = false;

    while (pos < source_len && (std::isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') {
            if (has_dot) {
                throw_error(file_name, LEXER, "Invalid number literal (" + std::to_string(line) + ':' + std::to_string(column) + ")\n", line);
            }
            has_dot = true;
        }
        val += advance();
    }
    if (has_dot) {
        return Token(TokenType::F64_LIT, val, tmp_l, tmp_c, file_name);
    }
    return Token(TokenType::I32_LIT, val, tmp_l, tmp_c, file_name);
}

Token Lexer::tokenize_string() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;

    advance();
    while (pos < source_len && peek() != '"') {
        if (peek() == '\\') {
            advance();
            val += advance_escape_sequence();
        }
        else {
            val += advance();
        }
    }
    advance();

    return Token(TokenType::STRING_LIT, val, tmp_l, tmp_c, file_name);
}

Token Lexer::tokenize_char() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;

    advance();
    while (pos < source_len && peek() != '\'') {
        if (peek() == '\\') {
            advance();
            val += advance_escape_sequence();
        }
        else {
            val += advance();
        }
    }
    advance();

    return Token(TokenType::I8_LIT, val, tmp_l, tmp_c, file_name);
}

Token Lexer::tokenize_id_or_keyword() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;

    while (pos < source_len && (std::isalpha(peek()) || peek() == '_' || std::isdigit(peek()))) {
        val += advance();
    }

    if (keywords.find(val) != keywords.end()) {
        return Token(keywords[val], val, tmp_l, tmp_c, file_name);
    }
    else if (val == "true" || val == "false") {
        return Token(TokenType::BOOL_LIT, val, tmp_l, tmp_c, file_name);
    }
    return Token(TokenType::ID, val, tmp_l, tmp_c, file_name);
}

Token Lexer::tokenize_op() {
    const char c = peek();
    int tmp_l = line;
    int tmp_c = column;
    switch (c) {
        case '(':
            advance();
            return Token(TokenType::LPAREN, "(", tmp_l, tmp_c, file_name);
        case ')':
            advance();
            return Token(TokenType::RPAREN, ")", tmp_l, tmp_c, file_name);
        case '[':
            advance();
            return Token(TokenType::LBRACKET, "{", tmp_l, tmp_c, file_name);
        case ']':
            advance();
            return Token(TokenType::RBRACKET, "}", tmp_l, tmp_c, file_name);
        case '{':
            advance();
            return Token(TokenType::LBRACE, "{", tmp_l, tmp_c, file_name);
        case '}':
            advance();
            return Token(TokenType::RBRACE, "}", tmp_l, tmp_c, file_name);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", tmp_l, tmp_c, file_name);
        case ':':
            advance();
            return Token(TokenType::COLON, ":", tmp_l, tmp_c, file_name);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", tmp_l, tmp_c, file_name);
        case '.':
            advance();
            return Token(TokenType::DOT, ".", tmp_l, tmp_c, file_name);
        case '?':
            advance();
            return Token(TokenType::QUESTION, "?", tmp_l, tmp_c, file_name);
        case '+':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::PLUS_EQ, "+=", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::PLUS, "+", tmp_l, tmp_c, file_name);
        case '-':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::MINUS_EQ, "-=", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::MINUS, "-", tmp_l, tmp_c, file_name);
        case '*':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::MULT_EQ, "*=", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::MULT, "*", tmp_l, tmp_c, file_name);
        case '/':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::DIV_EQ, "/=", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::DIV, "/", tmp_l, tmp_c, file_name);
        case '%':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::MODULO_EQ, "%=", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::MODULO, "%", tmp_l, tmp_c, file_name);
        case '=':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQ_EQ, "==", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::EQ, "=", tmp_l, tmp_c, file_name);
        case '!':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQ, "!=", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::L_NOT, "!", tmp_l, tmp_c, file_name);
        case '~':
            advance();
            return Token(TokenType::B_NOT, "~", tmp_l, tmp_c, file_name);
        case '>':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::GT_EQ, ">=", tmp_l, tmp_c, file_name);
            }
            else if (peek() == '>') {
                advance();
                return Token(TokenType::R_SHIFT, ">>", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::GT, ">", tmp_l, tmp_c, file_name);
        case '<':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::LS_EQ, "<=", tmp_l, tmp_c, file_name);
            }
            else if (peek() == '<') {
                advance();
                return Token(TokenType::L_SHIFT, "<<", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::LS, "<", tmp_l, tmp_c, file_name);
        case '&':
            advance();
            if (peek() == '&') {
                advance();
                return Token(TokenType::L_AND, "&&", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::B_AND, "&", tmp_l, tmp_c, file_name);
        case '|':
            advance();
            if (peek() == '|') {
                advance();
                return Token(TokenType::L_OR, "||", tmp_l, tmp_c, file_name);
            }
            return Token(TokenType::B_OR, "|", tmp_l, tmp_c, file_name);
        case '^':
            advance();
            return Token(TokenType::B_XOR, "^", tmp_l, tmp_c, file_name);
        default:
            throw_error(file_name, LEXER, "Unsupported operator: '" + std::string{1, c} + "' (" + std::to_string(line) + ':' + std::to_string(column) + ")\n", line);
    }
}

void Lexer::skip_singleline_comment() {
    while (peek() != '\n') {
        advance();
    }
    advance();
}

void Lexer::skip_multiline_comment() {
    advance();
    advance();
    while (peek() != '/' || peek(-1) != '*') {
        advance();
    }
    advance();
}

void Lexer::handle_preprocessor() {
    while (peek() == ' ') {
        advance();
    }
    std::string directive_name;
    while (peek() != ' ') {
        directive_name += advance();
    }
    if (directive_name == "include") {
        handle_preprocessor_include();
    }
    else {
        throw_error(file_name, LEXER, "Unsupported preprocessor directive: '" + directive_name + "' (" + std::to_string(line) + ':' + std::to_string(column) + ")\n", line);
    }
}

void Lexer::handle_preprocessor_include() {
    while (peek() == ' ') {
        advance();
    }
    if (peek() != '<') {
        throw_error(file_name, LEXER, "Use: 'include <include_name>' (" + std::to_string(line) + ':' + std::to_string(column) + ")\n", line);
    }
    advance();
    int tmp_l = line;
    int tmp_c = column;
    std::string include_file_name;
    while (peek() != '\n' && peek() != '>') {
        include_file_name += advance();
    }
    include_file_name += ".bl";
    if (peek() != '>') {
        throw_error(file_name, LEXER, "Use: 'include <include_name>' (" + std::to_string(line) + ':' + std::to_string(column) + ")\n", line);
    }
    advance();

    std::filesystem::path absolute_current_file_path(this->file_name);
    std::string absolute_include_file_path = absolute_current_file_path.parent_path().string() + '/' + include_file_name;
    if (std::find(Lexer::included_files_paths.begin(), Lexer::included_files_paths.end(), absolute_include_file_path) != Lexer::included_files_paths.end()) {
        return;
    }
    std::ifstream file(absolute_include_file_path);
    if (!file.is_open()) {
        throw_error(file_name, LEXER, "File '" + include_file_name + "' in '" + absolute_current_file_path.parent_path().string() + "/' does not exist (" + std::to_string(tmp_l) + ':' + std::to_string(tmp_c) + ")\n", tmp_l);
    }
    Lexer::included_files_paths.push_back(absolute_include_file_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Lexer include_lexer(content, absolute_include_file_path);
    std::vector<Token> include_tokens = include_lexer.tokenize();
    unsigned include_tokens_size = include_tokens.size();
    for (unsigned i = 0; i < include_tokens_size; i++) {
        tokens.push_back(include_tokens[i]);
    }
}

const char Lexer::advance_escape_sequence() {
    const char c = advance();
    switch (c) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case '\\':
            return '\\';
        case '\"':
            return '\"';
        case '\'':
            return '\'';
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'r':
            return '\r';
        case 'f':
            return '\f';
        case 'v':
            return '\v';
        default:
            throw_error(file_name, LEXER, "Unsupported escape-sequence: '\\" + std::string{1, c} + "' (" + std::to_string(line) + ':' + std::to_string(column - 2) + ")\n", line);
    }
}

const char Lexer::peek(int rpos) {
    if (pos + rpos >= source_len) {
        throw_error(file_name, LEXER, "Index out of range: (" + std::to_string(pos + rpos) + "/" + std::to_string(source_len) + ")\n", line);
    }
    return source[pos + rpos];
}

const char Lexer::advance() {
    const char c = peek();
    column++;
    pos++;
    if (c == '\n') {
        line++;
        column = 1;
    }
    return c;
}

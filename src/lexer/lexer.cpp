#include "../../include/lexer/lexer.hpp"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

std::map<std::string, TokenType> keywords = {
    {"char", TokenType::CHAR},
    {"short", TokenType::SHORT},
    {"int", TokenType::INT},
    {"long", TokenType::LONG},
    {"float", TokenType::FLOAT},
    {"double", TokenType::DOUBLE},
    {"void", TokenType::VOID},

    {"signed", TokenType::SIGNED},
    {"unsigned", TokenType::UNSIGNED},
    {"typedef", TokenType::TYPEDEF},
    {"struct", TokenType::STRUCT},
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
    {"const", TokenType::CONST},
};

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < source_len) {
        const char c = peek();
        if (c == '\n' || c == ' ') {
            advance();
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
                std::cerr << "Invalid number literal (twice dot)\n";
                exit(1);
            }
            has_dot = true;
        }
        val += advance();
    }

    if (has_dot) {
        return Token(TokenType::DOUBLE_LIT, val, tmp_l, tmp_c);
    }
    return Token(TokenType::INT_LIT, val, tmp_l, tmp_c);
}

Token Lexer::tokenize_string() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;

    advance();
    while (pos < source_len && peek() != '"') {
        val += advance();
    }
    advance();

    return Token(TokenType::STRING_LIT, val, tmp_l, tmp_c);
}

Token Lexer::tokenize_char() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;

    advance();
    while (pos < source_len && peek() != '\'') {
        val += advance();
    }
    advance();

    return Token(TokenType::CHAR_LIT, std::string{1, val[0]}, tmp_l, tmp_c);
}

Token Lexer::tokenize_id_or_keyword() {
    std::string val;
    int tmp_l = line;
    int tmp_c = column;

    while (pos < source_len && (std::isalpha(peek()) || peek() == '_' || std::isdigit(peek()))) {
        val += advance();
    }

    if (keywords.find(val) != keywords.end()) {
        return Token(keywords[val], val, tmp_l, tmp_c);
    }
    return Token(TokenType::ID, val, tmp_l, tmp_c);
}

Token Lexer::tokenize_op() {
    const char c = peek();
    int tmp_l = line;
    int tmp_c = column;
    switch (c) {
        case '(':
            advance();
            return Token(TokenType::LPAREN, "(", tmp_l, tmp_c);
        case ')':
            advance();
            return Token(TokenType::RPAREN, ")", tmp_l, tmp_c);
        case '[':
            advance();
            return Token(TokenType::LBRACKET, "{", tmp_l, tmp_c);
        case ']':
            advance();
            return Token(TokenType::RBRACKET, "}", tmp_l, tmp_c);
        case '{':
            advance();
            return Token(TokenType::LBRACE, "{", tmp_l, tmp_c);
        case '}':
            advance();
            return Token(TokenType::RBRACE, "}", tmp_l, tmp_c);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", tmp_l, tmp_c);
        case ':':
            advance();
            return Token(TokenType::COLON, ":", tmp_l, tmp_c);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", tmp_l, tmp_c);
        case '.':
            advance();
            return Token(TokenType::DOT, ".", tmp_l, tmp_c);
        case '?':
            advance();
            return Token(TokenType::QUESTION, "?", tmp_l, tmp_c);
        case '+':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::PLUS_EQ, "+=", tmp_l, tmp_c);
            }
            return Token(TokenType::PLUS, "+", tmp_l, tmp_c);
        case '-':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::MINUS_EQ, "-=", tmp_l, tmp_c);
            }
            return Token(TokenType::MINUS, "-", tmp_l, tmp_c);
        case '*':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::MULT_EQ, "*=", tmp_l, tmp_c);
            }
            return Token(TokenType::MULT, "*", tmp_l, tmp_c);
        case '/':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::DIV_EQ, "/=", tmp_l, tmp_c);
            }
            return Token(TokenType::DIV, "/", tmp_l, tmp_c);
        case '%':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::MODULO_EQ, "%=", tmp_l, tmp_c);
            }
            return Token(TokenType::MODULO, "%", tmp_l, tmp_c);
        case '=':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQ_EQ, "==", tmp_l, tmp_c);
            }
            return Token(TokenType::EQ, "=", tmp_l, tmp_c);
        case '!':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQ, "!=", tmp_l, tmp_c);
            }
            return Token(TokenType::L_NOT, "!", tmp_l, tmp_c);
        case '~':
            advance();
            return Token(TokenType::B_NOT, "~", tmp_l, tmp_c);
        case '>':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::GT_EQ, ">=", tmp_l, tmp_c);
            }
            else if (peek() == '>') {
                advance();
                return Token(TokenType::R_SHIFT, ">>", tmp_l, tmp_c);
            }
            return Token(TokenType::GT, ">", tmp_l, tmp_c);
        case '<':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::LS_EQ, "<=", tmp_l, tmp_c);
            }
            else if (peek() == '<') {
                advance();
                return Token(TokenType::L_SHIFT, "<<", tmp_l, tmp_c);
            }
            return Token(TokenType::LS, "<", tmp_l, tmp_c);
        case '&':
            advance();
            if (peek() == '&') {
                advance();
                return Token(TokenType::L_AND, "&&", tmp_l, tmp_c);
            }
            return Token(TokenType::B_AND, "&", tmp_l, tmp_c);
        case '|':
            advance();
            if (peek() == '|') {
                advance();
                return Token(TokenType::L_OR, "||", tmp_l, tmp_c);
            }
            return Token(TokenType::B_OR, "|", tmp_l, tmp_c);
        case '^':
            advance();
            return Token(TokenType::B_XOR, "^", tmp_l, tmp_c);
        default:
            std::cerr << std::string("Unsupported operator: '") + c + "'";
            exit(1);
    }
}

const char Lexer::peek(int rpos) const {
    if (pos + rpos >= source_len) {
        std::cerr << "Index out of range: (" + std::to_string(pos + rpos) + "/" + std::to_string(source_len) + ")\n";
        exit(1);
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

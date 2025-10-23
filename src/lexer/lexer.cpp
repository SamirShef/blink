#include "../../include/lexer/lexer.hpp"
#include <stdexcept>
#include <cctype>

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
    bool has_dot;

    while (pos < source_len && (std::isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') {
            if (has_dot) {
                throw std::runtime_error("Invalid number literal (twice dot)");
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
    //return Token();
}

Token Lexer::tokenize_char() {
    //return Token();
}

Token Lexer::tokenize_id_or_keyword() {
    //return Token();
}

Token Lexer::tokenize_op() {
    //return Token();
}

const char Lexer::peek(int rpos) const {
    if (pos + rpos >= source_len) {
        throw std::runtime_error("Index out of range: (" + std::to_string(pos + rpos) + "/" + std::to_string(source_len) + ")");
    }
    return source[pos + rpos];
}

const char Lexer::advance() {
    const char c = peek();
    pos++;
    return c;
}

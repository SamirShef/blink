#pragma once
#include "token.hpp"
#include <string>
#include <vector>

class Lexer {
private:
    std::string source;
    unsigned long source_len;
    int pos;
    int line;
    int column;

public:
    Lexer(std::string s) : source(s), source_len(s.length()), pos(0), line(1), column(1) {}

    std::vector<Token> tokenize();

private:
    Token tokenize_number();
    Token tokenize_string();
    Token tokenize_char();
    Token tokenize_id_or_keyword();
    Token tokenize_op();
    void skip_singleline_comment();
    void skip_multiline_comment();

    const char advance_escape_sequence();
    const char peek(int rpos = 0) const;
    const char advance();
};

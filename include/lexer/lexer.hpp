#pragma once
#include "token.hpp"
#include <vector>

class Lexer {
private:
    std::string source;
    unsigned long source_len;
    int pos;
    int line;
    int column;
    std::string file_name;
    bool file_name_in_error_printed;

public:
    Lexer(std::string s, std::string fn) : source(s), source_len(s.length()), pos(0), line(1), column(1), file_name(fn), file_name_in_error_printed(false) {}

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
    const char peek(int rpos = 0);
    const char advance();
};

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
    std::vector<Token> tokens;
    std::string file_name;

public:
    static std::vector<std::string> included_files_paths;

    Lexer(std::string s, std::string fn) : source(s), source_len(s.length()), pos(0), line(1), column(1), file_name(fn), tokens({}) {}

    std::vector<Token> tokenize();

private:
    Token tokenize_number();
    Token tokenize_string();
    Token tokenize_char();
    Token tokenize_id_or_keyword();
    Token tokenize_op();

    void skip_singleline_comment();
    void skip_multiline_comment();
    void handle_preprocessor();
    void handle_preprocessor_include();

    const char advance_escape_sequence();
    const char peek(int rpos = 0);
    const char advance();
};

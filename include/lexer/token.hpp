#pragma once
#include <string>

enum class TokenType {
    I8,
    I16,
    I32,
    I64,
    F32,
    F64,
    U8,
    U16,
    U32,
    U64,
    BOOL,
    NOTHING,

    VAR,
    CONST,
    FUNC,
    CLASS,
    ENUM,
    SIZEOF,
    IF,
    ELSE,
    WHILE,
    DO,
    FOR,
    BREAK,
    CONTINUE,
    RETURN,

    LPAREN,		                        // (
    RPAREN,		                        // )
    LBRACKET,		                    // [
    RBRACKET,		                    // ]
    LBRACE,		                        // {
    RBRACE,		                        // }
    SEMICOLON,                          // ;
    COLON,                              // :
    COMMA,                              // ,
    DOT,                                // .
    QUESTION,                           // ?

    PLUS,		                        // +
    MINUS,		                        // -
    MULT,		                        // *
    DIV,		                        // /
    MODULO,		                        // %
    PLUS_EQ,		                    // +=
    MINUS_EQ,		                    // -=
    MULT_EQ,		                    // *=
    DIV_EQ,		                        // /=
    MODULO_EQ,		                    // %=
    EQ,			                        // =
    EQ_EQ,		                        // ==
    L_NOT,                              // !
    B_NOT,                              // ~
    NOT_EQ,		                        // !=
    GT,			                        // >
    GT_EQ,		                        // >=
    LS,			                        // <
    LS_EQ,		                        // <=
    L_AND,		                        // &&
    B_AND,                              // &
    L_OR,		                        // ||
    B_OR,		                        // |
    B_XOR,                              // |
    R_SHIFT,                            // >>
    L_SHIFT,                            // <<

    I8_LIT,
    I16_LIT,
    I32_LIT,
    I64_LIT,
    F32_LIT,
    F64_LIT,
    U8_LIT,
    U16_LIT,
    U32_LIT,
    U64_LIT,
    BOOL_LIT,
    STRING_LIT,

    ID,
};

struct Token {
    TokenType type;
    std::string value;

    int line;
    int column;

    std::string file_name;

    Token(TokenType t, std::string v, int l, int c, std::string fn) : type(t), value(v), line(l), column(c), file_name(fn) {}
};

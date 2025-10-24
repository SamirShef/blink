#include <string>

enum class TokenType {
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    VOID,

    SIGNED,
    UNSIGNED,
    TYPEDEF,
    STRUCT,
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
    CONST,

    LPAREN,		                        // (
    RPAREN,		                        // )
    LBRACKET,		                    // [
    RBRACKET,		                    // ]
    LBRACE,		                        // {
    RBRACE,		                        // }
    SEMICOLON,                          // ;
    COLON,                              // :

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

    CHAR_LIT,
    SHORT_LIT,
    INT_LIT,
    LONG_LIT,
    FLOAT_LIT,
    DOUBLE_LIT,
    STRING_LIT,

    ID,
};

struct Token {
    TokenType type;
    std::string value;

    int line;
    int column;

    Token(TokenType t, std::string v, int l, int c) : type(t), value(v), line(l), column(c) {}
};

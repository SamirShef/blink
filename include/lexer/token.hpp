#include <string>

enum class TokenType { SIGNED, UNSIGNED, 
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    VOID,
 
    TYPEDEF,
    STRUCT,
  
    SIZEOF,
    IF,
    ELSE,
    WHILE, 
    DO,
    FOR,
    BREAK,
    CONTINUE,

    LPAREN,		// (
    RPAREN,		// )
    LBRACKET,		// [
    RBRACKET,		// ]
    LBRACE,		// {
    RBRACE,		// }

    PLUS,		// +
    MINUS,		// -
    MULT,		// *
    DIV,		// /
    MODULO,		// %
    PLUS_EQ,		// +=
    MINUS_EQ,		// -=
    MULT_EQ,		// *=
    DIV_EQ,		// /=
    MODULO_EQ,		// %=
    EQ,			// =
    EQ_EQ,		// ==
    NOT_EQ,		// !=
    GT,			// >
    GT_EQ,		// >=
    LS,			// <
    LS_EQ,		// <=
    L_AND,		// &&
    L_OR,		// ||

    CHAR_LIT,
    SHORT_LIT,
    INT_LIT,
    LONG_LIT,
    FLOAT_LIT,
    DOUBLE_LIT,
    STRING_LIT,
};

struct Token {
    TokenType type;
    std::string value;

    int line;
    int column;

    Token(TokenType t, std::string v, int l, int c) : type(t), value(v), line(l), column(c) {}
};

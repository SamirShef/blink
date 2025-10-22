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
};

struct Token {
  TokenType type;
  std::string value;

  int line;
  int column;
};

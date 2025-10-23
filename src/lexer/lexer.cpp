#include "../../include/lexer/lexer.hpp"
#include <stdexcept>

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  return tokens;
}

Token Lexer::tokenize_number() {
  //return Token();
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

#pragma once

#include "TypeDefs.h"

namespace VeNo::VUI {

enum class TokenType : int {
  Keyword = 0, // loop and internal identifier
  Identifier,
  Number,

  Colon, // Identifier -> Value
  Minus,
  Plus,
  Multiply,
  Slash,
  Percent,
  StringLiteral,
  Component, // @
  LeftCurlyBrace,
  RightCurlyBrace,
  LeftParenthesis,
  RightParenthesis,
  Semicolon,
  DollarSign, // For String Interpolation
  Invalid = 0xd3ad,
  EndOfFile
};

static const char *TokenTypeToString(TokenType type) {
  switch (type) {
  case TokenType::Keyword: return "TokenType::Keyword";
  case TokenType::Identifier: return "TokenType::Identifier";
  case TokenType::Number: return "TokenType::Number";
  case TokenType::Colon: return "TokenType::Colon";
  case TokenType::Minus: return "TokenType::Minus";
  case TokenType::Plus: return "TokenType::Plus";
  case TokenType::Multiply: return "TokenType::Multiply";
  case TokenType::Slash: return "TokenType::Slash";
  case TokenType::Percent: return "TokenType::Percent";
  case TokenType::StringLiteral: return "TokenType::StringLiteral";
  case TokenType::Component: return "TokenType::Component";
  case TokenType::LeftCurlyBrace: return "TokenType::LeftCurlyBrace";
  case TokenType::RightCurlyBrace: return "TokenType::RightCurlyBrace";
  case TokenType::LeftParenthesis: return "TokenType::LeftParenthesis";
  case TokenType::RightParenthesis: return "TokenType::RightParenthesis";
  case TokenType::Semicolon: return "TokenType::Semicolon";
  case TokenType::DollarSign: return "TokenType::DollarSign";
  case TokenType::Invalid: return "TokenType::Invalid";
  case TokenType::EndOfFile: return "TokenType::EndOfFile";
  }
  return "TokenType::Unknown";
}

struct Token {
  std::string_view Value;
  TokenType Type = TokenType::Invalid;
};

struct TokenStream {
  std::vector<Token> Tokens;
  u32 Position{0};
  const Token &Consume();
  Token Peek(u32 offset = 0);
  TokenType PeekType(u32 offset = 0);
  void ConsumeIfMatch(TokenType type);

  bool IsNotEnd() {
    return Position != Tokens.size() - 1 &&
           Tokens[Position].Type != TokenType::EndOfFile;
  }

  void AddToken(Token token) { Tokens.push_back(token); }

  bool IsValid() { return !m_IsInvalid; }

  void SetIsValid(bool validOrNot);

protected:
  bool m_IsInvalid;
};
} // namespace VeNo::VUI
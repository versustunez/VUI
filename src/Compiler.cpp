#include "Compiler.h"

#include <fstream>
#include <iostream>

namespace VeNo::VUI {

static bool ReadFile(const std::string &file, std::string &result) {
  std::ifstream in(file, std::ios::in | std::ios::binary);
  if (in) {
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    if (size != -1) {
      result.resize(size);
      in.seekg(0, std::ios::beg);
      in.read(&result[0], size);
    } else {
      return false;
    }
  } else {
    return false;
  }
  return true;
}

Compiler::Compiler(const std::string &filePath) : m_File(filePath) {
  if (!ReadFile(filePath, m_FileContent)) {
    std::cerr << "Failed to read file " << filePath << std::endl;
    return;
  };
  m_Stream = StringStream{std::string_view(m_FileContent)};
}

void Compiler::Parse() {
  while (!m_Stream.IsEndOfStream()) {
    m_Stream.SkipWhitespaces();
    char token = m_Stream.Advance();
    HandleChar(token);
  }
  MakeToken(TokenType::EndOfFile);
}

void Compiler::MakeToken(TokenType type, u32 begin, u32 end) {
  m_Tokens.AddToken({m_Stream.GetView(begin, end), type});
}

void Compiler::MakeToken(TokenType type) {
  u32 pos = m_Stream.GetPosition();
  m_Tokens.AddToken({m_Stream.GetView(pos, pos + 1), type});
}

void Compiler::HandleChar(char token) {
  switch (token) {
  case '@': return MakeToken(TokenType::Component);
  case '{': return MakeToken(TokenType::LeftCurlyBrace);
  case '}': return MakeToken(TokenType::RightCurlyBrace);
  case '(': return MakeToken(TokenType::LeftParenthesis);
  case ')': return MakeToken(TokenType::RightParenthesis);
  case ';': return MakeToken(TokenType::Semicolon);
  case '/':
    return m_Stream.Peek() == '/' ? m_Stream.SkipWhileNot('\n')
                                  : MakeToken(TokenType::Slash);
  case '%': return MakeToken(TokenType::Percent);
  case '"': {
    u32 pos = m_Stream.GetPosition();
    return MakeToken(TokenType::StringLiteral, pos, m_Stream.ExtractString());
  }

  case '*': return MakeToken(TokenType::Multiply);
  case '-': return MakeToken(TokenType::Minus);
  case '+': return MakeToken(TokenType::Plus);
  case ':': return MakeToken(TokenType::Colon);
  case '$': return MakeToken(TokenType::DollarSign);
  }

  if (StringStream::IsNumeric(token)) {
    u32 pos = m_Stream.GetCurrentPosition();
    u32 end = m_Stream.ExtractNumber();
    return MakeToken(TokenType::Number, pos, end);
  }

  if (StringStream::IsAlpha(token)) {
    u32 pos = m_Stream.GetCurrentPosition();
    u32 end = m_Stream.ExtractIdentifier();

    if (IsValidKeyword(m_Stream.GetView(pos, end)))
      return MakeToken(TokenType::Keyword, pos, end);
    return MakeToken(TokenType::Identifier, pos, end);
  }
}

bool Compiler::IsValidKeyword(std::string_view identifier) {
  if (identifier == "loop")
    return true;
  if (identifier == "include")
    return true;
  return false;
}

void Compiler::DumpTokens() {
  for (auto &token : m_Tokens.Tokens)
    std::cout << TokenTypeToString(token.Type) << " >> " << token.Value << "\n";
}
const StringStream &Compiler::GetStream() { return m_Stream; }
} // namespace VeNo::VUI
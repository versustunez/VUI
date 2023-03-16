#pragma once

#include "InterpretedComponent.h"
#include "StringStream.h"
#include "Token.h"
#include "TypeDefs.h"

#include <vector>

namespace VeNo::VUI {

class Compiler {
public:
  explicit Compiler(const std::string &filename);
  void Parse();
  void DumpTokens();
  TokenStream &GetTokenStream() { return m_Tokens; }

  const StringStream &GetStream();

protected:
  void HandleChar(char);
  void MakeToken(TokenType type, u32 begin, u32 end);
  void MakeToken(TokenType type);
  static bool IsValidKeyword(std::string_view identifier);

protected:
  std::string m_File{};
  StringStream m_Stream{""};
  std::string m_FileContent{};
  TokenStream m_Tokens{};
};

} // namespace VeNo::VUI

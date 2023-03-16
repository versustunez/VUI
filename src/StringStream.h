#pragma once

#include "TypeDefs.h"

#include <iostream>
#include <string_view>

namespace VeNo::VUI {

class StringStream {
public:
  explicit StringStream(std::string_view content) : m_Stream(content) {}
  char Advance() {
    return m_Position >= m_Stream.size() ? '\0' : m_Stream[m_Position++];
  }
  char Peek(int offset = 0) {
    return m_Position + offset > m_Stream.size()
               ? '\0'
               : m_Stream[m_Position + offset];
  }

  void Skip(int count = 1) {
    for (int i = 0; i < count; ++i)
      Advance();
  }

  void SkipIfMatch(char match) {
    if (Peek() == match)
      Advance();
  }

  void SkipWhile(char ch) {
    while (Peek() == ch && !IsEndOfStream())
      Advance();
  }

  void SkipWhitespaces() {
    while (Peek() == ' ' || Peek() == '\t' || Peek() == '\n')
      Advance();
  }

  bool IsEndOfStream() {
    return m_Position > m_Stream.size() || m_Stream[m_Position] == '\0';
  }

  u32 GetPosition() { return m_Position; }
  u32 GetCurrentPosition() { return m_Position - 1; }

  static bool IsAlphaNumeric(char ch) { return isalnum(ch); }
  static bool IsAlpha(char ch) { return isalpha(ch); }
  static bool IsNumeric(char ch) { return isdigit(ch); }

  u32 ExtractString() {
    char ch = Peek();
    while (!(ch == '"' || ch == '\0')) {
      ch = Advance();
    }
    return IsEndOfStream() ? 0 : m_Position-1;
  }

  u32 ExtractNumber() {
    while (IsNumeric(Peek())) {
      Advance();
    }
    return m_Position;
  }

  u32 ExtractIdentifier() {
    while (IsAlphaNumeric(Peek())) {
      Advance();
    }
    return m_Position;
  }

  [[nodiscard]] std::string_view GetView(u32 start, u32 end) const {
    u32 n = end - start;
    if (n == 0)
      n = 1;
    auto sub = m_Stream.substr(start, n);
    return sub;
  }

protected:
  std::string_view m_Stream{};
  u32 m_Position{0};
};
} // namespace VeNo::VUI
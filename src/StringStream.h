#pragma once

#include "TypeDefs.h"

#include <iostream>
#include <string_view>

namespace VUI {

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

  void SkipWhileNot(char ch) {
    while (Peek() != ch && !IsEndOfStream())
      Advance();
  }

  void SkipWhitespaces() {
    while (Peek() == ' ' || Peek() == '\t' || Peek() == '\n')
      Advance();
  }

  bool IsEndOfStream() {
    return m_Position > m_Stream.size() || m_Stream[m_Position] == '\0';
  }

  std::uint32_t GetPosition() { return m_Position; }
  std::uint32_t GetCurrentPosition() { return m_Position - 1; }

  static bool IsAlphaNumeric(char ch) { return isalnum(ch); }
  static bool IsAlpha(char ch) { return isalpha(ch); }
  static bool IsNumeric(char ch) { return isdigit(ch); }

  std::uint32_t ExtractString() {
    char ch = Peek();
    while (!(ch == '"' || ch == '\0')) {
      ch = Advance();
    }
    return IsEndOfStream() ? 0 : m_Position-1;
  }

  std::uint32_t ExtractNumber() {
    while (IsNumeric(Peek())) {
      Advance();
    }
    return m_Position;
  }

  std::uint32_t ExtractIdentifier() {
    while (IsAlphaNumeric(Peek())) {
      Advance();
    }
    return m_Position;
  }

  [[nodiscard]] std::string_view GetView(std::uint32_t start, std::uint32_t end) const {
    std::uint32_t n = end - start;
    if (n == 0)
      n = 1;
    auto sub = m_Stream.substr(start, n);
    return sub;
  }

protected:
  std::string_view m_Stream{};
  std::uint32_t m_Position{0};
};
} // namespace VUI
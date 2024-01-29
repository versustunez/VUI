#include "Token.h"

namespace VUI {
const Token &TokenStream::Consume() { return Tokens[Position++]; }
Token TokenStream::Peek(std::uint32_t offset) { return Tokens[Position + offset]; }
TokenType TokenStream::PeekType(std::uint32_t offset) {
  return Tokens[Position + offset].Type;
}
void TokenStream::ConsumeIfMatch(TokenType type) {
  if (PeekType(0) == type)
    Consume();
}
void TokenStream::SetIsValid(bool validOrNot) { m_IsInvalid = !validOrNot; }

} // namespace VUI
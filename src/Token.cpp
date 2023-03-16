#include "Token.h"

namespace VeNo::VUI {
const Token &TokenStream::Consume() { return Tokens[Position++]; }
Token TokenStream::Peek(u32 offset) { return Tokens[Position + offset]; }
TokenType TokenStream::PeekType(u32 offset) {
  return Tokens[Position + offset].Type;
}
void TokenStream::ConsumeIfMatch(TokenType type) {
  if (PeekType(0) == type)
    Consume();
}
void TokenStream::SetIsValid(bool validOrNot) { m_IsInvalid = !validOrNot; }

} // namespace VeNo::VUI